#pragma GCC optimize("O3")
#include "_svm.hpp"

using namespace svm;

SysVM *sysvm = nullptr;

// ======================= SysVMThread：执行状态 =======================

SysVMThread::SysVMThread(): stack{}, sp{0}, reg{} {}

void SysVMThread::reset() {
    // Result descriptors are carved out of stack slots. A token Value before
    // each descriptor identifies its kind and slot count; free the array that
    // the descriptor points at, then drop everything with sp = 0.
    for (unsigned i = 0; i < sp; ) {
        const Value &v = stack[i];
        if (v.type != Value::TypeID::Reg ||
            (v.value & ResultTokenMagic) != ResultTokenMagic) {
            i++;
            continue;
        }

        const unsigned kind = (v.value >> ResultKindShift) & 0xFFu;
        const unsigned slots = v.value & ResultSlotsMask;
        if (i + 1 + slots > sp) break;

        void *base = &stack[i + 1];
        switch (static_cast<ResultKind>(kind)) {
        case ResultKind::Read:
            delete[] reinterpret_cast<ReadResult *>(base)->addr;
            break;
        case ResultKind::Mem:
            delete[] reinterpret_cast<MemResult *>(base)->addr;
            break;
        case ResultKind::Obj:
            delete[] reinterpret_cast<ObjResult *>(base)->addr;
            break;
        case ResultKind::Fs:
            delete[] reinterpret_cast<FsResult *>(base)->addr;
            break;
        case ResultKind::Status: {
            auto *r = reinterpret_cast<StatusResult *>(base);
            delete[] r->objs;
            delete[] r->drives;
            break;
        }
        }
        i += 1 + slots;
    }

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

SysVM::SysVM() { sysvm = this; }

SysVMThread *SysVM::create_thread() { return new SysVMThread; }

bool SysVM::exec(SysVMThread &th, const Insn &in) {
    const unsigned index = static_cast<unsigned>(in.op);
    if (index >= OpcodeCount || !Opcode2Func[index]) return false;

    OpCtx ctx{th, th.stack, th.sp};
    return Opcode2Func[index](ctx, in);
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

    return true;
}
