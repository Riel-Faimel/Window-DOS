#include "_svm.hpp"

svm::SysVM *sysvm = nullptr;

using namespace svm;

SysVM::SysVM(): stack{}, sp{0} {
    sysvm = this;
}

void SysVM::push(u32 v) {
    if (sp < stack_max) stack[sp++] = v;
}

u32 SysVM::pop() {
    if (sp == 0) return 0;   // 前端按 argc 编译，正常到不了这里
    return stack[--sp];
}

bool SysVM::exec(const Insn &in) {
    switch (in.op) {
    case Op::Push: {
        push(in.imm);
        return true;
    }
    // read <disk#> <lba> [count] + 输出缓冲区：压栈顺序 buf, disk, lba, count。
    case Op::Read: {
        const unsigned cnt = pop();
        const unsigned lba = pop();
        const unsigned idx = pop();
        auto *out = reinterpret_cast<OutBuf *>(pop());
        if (!out) return false;

        DiskPick pick;
        pick.want = idx;
        kom->visit(pick_disk, &pick);
        if (!pick.disk) return true;   // 没有这块盘：输出为空

        const unsigned need = cnt * 512;
        if (out->cap - out->len < need) return false;
        unsigned got = pick.disk->read(out->data + out->len, lba, 0, cnt);
        if (got == (unsigned)-1) return false;
        out->len += got;
        return true;
    }
    // mem <addr> [count] + 输出缓冲区：压栈顺序 buf, addr, count。
    case Op::Mem: {
        const unsigned cnt = pop();
        const unsigned addr = pop();
        auto *out = reinterpret_cast<OutBuf *>(pop());
        if (!out) return false;
        return out->put(reinterpret_cast<const void *>(addr), cnt);
    }
    case Op::Obj: {
        auto *out = reinterpret_cast<OutBuf *>(pop());
        if (!out) return false;
        ObjWriter w{out};
        kom->visit(write_obj, &w);
        return true;
    }
    case Op::Fs: {
        auto *out = reinterpret_cast<OutBuf *>(pop());
        if (!out) return false;
        if (!driverletterspace) return true;
        for (auto &d : driverletterspace->space) {
            ObjRec rec{};
            rec.kind = RecKind::Drive;
            rec.a = d.driver_letter.empty() ? 0 : static_cast<u32>(d.driver_letter.c_str()[0]);
            rec.b = d.cid;
            if (!out->put_as(rec)) return false;
        }
        return true;
    }
    default:
        return false;
    }
}

void SysVM::run(const ShellNode &node) {
    if (!node.ok) {
        kout << node.error << '\n';
        return;
    }

    sp = 0;
    for (auto &in : node.code) {
        if (!exec(in)) {
            kout << "exec fault\n";
            break;
        }
    }
    sp = 0;
}
