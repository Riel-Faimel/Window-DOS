#include "_svm.hpp"
#include <interface/svm/data.hpp>

using namespace svm;

SysVM *sysvm = nullptr;

// ======================= SysVMThread：执行状态 =======================

SysVMThread::SysVMThread(): stack{}, sp{0}, reg{} {}

void SysVMThread::reset() {
    sp = 0;
    for (unsigned i = 0; i < reg_max; i++) reg[i] = Value{};
}

void SysVMThread::push(Value v) {
    if (sp < stack_max) stack[sp++] = v;
}

Value SysVMThread::pop() {
    if (sp == 0) return Value{};
    return stack[--sp];
}

// Ref / Reg 都是"位置"：Ref 指栈槽，Reg 指寄存器。
Value *SysVMThread::slot_of(const Value &where) {
    if (where.type == Value::TypeID::Ref) {
        if (where.value >= stack_max) return nullptr;
        return &stack[where.value];
    }
    if (where.type == Value::TypeID::Reg) {
        if (where.value >= reg_max) return nullptr;
        return &reg[where.value];
    }
    return nullptr;
}

Value SysVMThread::value_of(const Value &v) {
    if (Value *p = slot_of(v)) return *p;
    return v;
}

// ======================= SysVM：无状态执行器 =======================

SysVM::SysVM() {
    sysvm = this;
}

SysVMThread *SysVM::create_thread() {
    return new SysVMThread;
}

bool SysVM::exec(SysVMThread &th, const Insn &in) {
    switch (in.op) {
    // 压容许数据；操作数是寄存器时压它的内容。
    case Op::Push: {
        if (in.imm.type == Value::TypeID::Reg) {
            Value *p = th.slot_of(in.imm);
            if (!p) return false;
            th.push(*p);
        } else {
            th.push(in.imm);
        }
        return true;
    }
    // 赋值：imm 指向目标位置（Ref 栈槽 / Reg 寄存器），把栈顶写进去。
    case Op::Assign: {
        Value *target = th.slot_of(in.imm);
        if (!target) return false;
        if (!th.sp) return false;

        const Value top = th.pop();
        if (target->type != Value::TypeID::Nil && target->type != top.type)
            return false;
        *target = top;
        return true;
    }
    case Op::Drop: {
        if (!th.sp) return false;
        th.sp--;
        return true;
    }
    // 交换：赋值语义要求两边类型相同（还没有协变关系）。
    case Op::Swap: {
        if (th.sp < 2) return false;
        Value &a = th.stack[th.sp - 1];
        Value &b = th.stack[th.sp - 2];
        if (a.type != b.type) return false;
        const Value t = a;
        a = b;
        b = t;
        return true;
    }
    // 比较：左边是栈顶（Ref / Reg 解引用），右边是 imm（同样解引用）。
    // 只接受强序类型：Int 按有符号比，Buf / Obj / Ref 按无符号比；Ordering 无序。
    case Op::Cmp: {
        if (!th.sp) return false;

        const Value a = th.value_of(th.stack[th.sp - 1]);
        const Value b = th.value_of(in.imm);
        if (a.type != b.type) return false;

        s32 r;
        switch (a.type) {
        case Value::TypeID::Int: {
            const s32 x = static_cast<s32>(a.value);
            const s32 y = static_cast<s32>(b.value);
            r = (x < y) ? -1 : (x > y) ? 1 : 0;
            break;
        }
        case Value::TypeID::Buf:
        case Value::TypeID::Obj:
        case Value::TypeID::Ref: {
            r = (a.value < b.value) ? -1 : (a.value > b.value) ? 1 : 0;
            break;
        }
        default:
            return false;   // Ordering / Nil / Reg 不参与比较
        }

        th.sp--;   // 比较消耗栈顶
        th.push(Value{Value::TypeID::Ordering, static_cast<u32>(r)});
        return true;
    }
    // 盘号 -> 变量。imm 指向目标位置，把解析出的内核对象指针写进去。
    case Op::Open: {
        if (!th.sp || th.stack[th.sp - 1].type != Value::TypeID::Int) return false;
        Value *target = th.slot_of(in.imm);
        if (!target) return false;

        const unsigned idx = th.pop().value;

        DiskPick pick;
        pick.want = idx;
        kom->visit(pick_disk, &pick);
        if (!pick.disk) { *target = Value{Value::TypeID::Nil, 0}; return true; }

        *target = Value{Value::TypeID::Obj, reinterpret_cast<u32>(pick.disk)};
        return true;
    }
    // read：压栈顺序 buf(Buf), 对象的槽引用(Ref), lba(Int), count(Int)。
    // 成功把读到的字节数（Int）压回栈。
    case Op::Read: {
        if (th.sp < 4) return false;
        if (th.stack[th.sp - 1].type != Value::TypeID::Int)  return false;   // count
        if (th.stack[th.sp - 2].type != Value::TypeID::Int)  return false;   // lba
        if (th.stack[th.sp - 3].type != Value::TypeID::Ref)  return false;   // disk 槽
        if (th.stack[th.sp - 4].type != Value::TypeID::Buf)  return false;   // out

        const unsigned cnt = th.pop().value;
        const unsigned lba = th.pop().value;

        const unsigned slot = th.pop().value;
        if (slot >= SysVMThread::stack_max) return false;
        Value &obj = th.stack[slot];
        if (obj.type != Value::TypeID::Obj) return false;
        auto *disk = reinterpret_cast<PhysicalDisk *>(obj.value);

        auto *out = reinterpret_cast<OutBuf *>(th.pop().value);
        if (!disk || !out) return false;

        const unsigned need = cnt * 512;
        if (out->cap - out->len < need) return false;

        const unsigned got = disk->read(out->data + out->len, lba, 0, cnt);
        if (got == (unsigned)-1) return false;
        out->len += got;

        th.push(Value{Value::TypeID::Int, got});
        return true;
    }
    // mem：压栈顺序 buf(Buf), addr(Int), count(Int)。
    case Op::Mem: {
        if (th.sp < 3) return false;
        if (th.stack[th.sp - 1].type != Value::TypeID::Int) return false;   // count
        if (th.stack[th.sp - 2].type != Value::TypeID::Int) return false;   // addr
        if (th.stack[th.sp - 3].type != Value::TypeID::Buf) return false;   // out

        const unsigned cnt = th.pop().value;
        const unsigned addr = th.pop().value;
        auto *out = reinterpret_cast<OutBuf *>(th.pop().value);
        if (!out) return false;

        const unsigned before = out->len;
        if (!out->put(reinterpret_cast<const void *>(addr), cnt)) return false;

        th.push(Value{Value::TypeID::Int, out->len - before});
        return true;
    }
    case Op::Obj: {
        if (!th.sp || th.stack[th.sp - 1].type != Value::TypeID::Buf) return false;
        auto *out = reinterpret_cast<OutBuf *>(th.pop().value);
        if (!out) return false;

        const unsigned before = out->len;
        ObjWriter w{out};
        kom->visit(write_obj, &w);

        th.push(Value{Value::TypeID::Int, out->len - before});
        return true;
    }
    case Op::Fs: {
        if (!th.sp || th.stack[th.sp - 1].type != Value::TypeID::Buf) return false;
        auto *out = reinterpret_cast<OutBuf *>(th.pop().value);
        if (!out) return false;

        const unsigned before = out->len;
        if (driverletterspace) {
            for (auto &d : driverletterspace->space) {
                ObjRec rec{};
                rec.kind = RecKind::Drive;
                rec.a = d.driver_letter.empty() ? 0 : static_cast<u32>(d.driver_letter.c_str()[0]);
                rec.b = d.cid;
                if (!out->put_as(rec)) return false;
            }
        }

        th.push(Value{Value::TypeID::Int, out->len - before});
        return true;
    }
    default:
        return false;
    }
}

bool SysVM::run(SysVMThread &th, const ShellNode &node) {
    if (!node.ok) {
        kout << node.error << '\n';
        return false;
    }

    const unsigned n = node.code.get_size();
    if (n == 0) return true;

    const Insn *code = &node.code[0];
    th.reset();

    unsigned pc = 0;
    while (pc < n) {
        const Insn &in = code[pc];

        switch (in.op) {
        case Op::Jmp:
        case Op::Jc: {
            u32 target = in.imm.value;
            bool take = true;

            // Jc 不弹任何东西：栈顶是 Ordering，下面一层是目标地址（Int）。
            if (in.op == Op::Jc) {
                if (in.imm.type != Value::TypeID::Ordering) return false;
                if (th.sp < 2 ||
                    th.stack[th.sp - 1].type != Value::TypeID::Ordering ||
                    th.stack[th.sp - 2].type != Value::TypeID::Int)
                    return false;

                const s32 r = static_cast<s32>(th.stack[th.sp - 1].value);
                take = (r == static_cast<s32>(in.imm.value));
                target = th.stack[th.sp - 2].value;
            }

            if (!take) { pc++; break; }

            if (target >= n) {
                kout << "bad jump: " << target << '\n';
                th.reset();
                return false;
            }
            pc = target;
            break;
        }
        default:
            if (!exec(th, in)) {
                kout << "exec fault\n";
                th.reset();
                return false;
            }
            pc++;
            break;
        }
    }

    th.reset();
    return true;
}
