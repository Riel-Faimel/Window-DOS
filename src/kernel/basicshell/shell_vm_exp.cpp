#include "shell_vm.hpp"
#include "shell_in.hpp"
#include <stdio>
#include <interface/knlobj/knlobj.hpp>
#include <KrnlObjMgr/kom.hpp>
#include <SrvDevMgr/DLS.hpp>

namespace {

void print_block(unsigned addr, unsigned len) {
    auto p = reinterpret_cast<volatile u8 *>(addr);
    for (unsigned base = 0; base < len; base += 16) {
        kout << "0x"; kout.hex(addr + base); kout << ": ";
        for (unsigned i = 0; i < 16; i++) {
            if (base + i < len) { kout.hex((unsigned char)p[base + i]); kout << ' '; }
            else kout << "   ";
        }
        kout << '|';
        for (unsigned i = 0; i < 16 && base + i < len; i++) {
            char ch = char(p[base + i]);
            kout << ((ch >= 0x20 && ch < 0x7F) ? ch : '.');
        }
        kout << "|\n";
    }
}

struct DiskPick {
    unsigned want = 0;
    unsigned seen = 0;
    PhysicalDisk *disk = nullptr;
};

void pick_disk(KernelObj *o, void *ctx) {
    auto *p = static_cast<DiskPick *>(ctx);
    auto *disk = dynamic_cast<PhysicalDisk *>(o);
    if (!disk) return;
    if (p->seen++ == p->want) p->disk = disk;
}

void dump_obj(KernelObj *o, void *) {
    if (auto *p = dynamic_cast<PhysicalDisk *>(o)) {
        kout << "[dev] phys " << (void *)p << '\n';
        return;
    }
    if (auto *d = dynamic_cast<LogicalDisk *>(o)) {
        auto *i = d->info();
        kout << "[dev] part  lba=" << i->su_start
             << " sectors=" << i->su_num.low << " type=0x";
        kout.hex((unsigned)i->type);
        kout << '\n';
        return;
    }
    if (dynamic_cast<SerialPort *>(o)) { kout << "[dev] serial\n"; return; }
    if (dynamic_cast<FramePort *>(o))  { kout << "[dev] frame\n";  return; }
    kout << "[dev] unknow\n";
}

} // namespace

// 一页 = 16 行 × 16 字节；多于一页就分页，按键交给前端。
void ShellVM::dump_bytes(unsigned addr, unsigned count) {
    constexpr unsigned page = 16 * 16;
    if (count == 0) return;

    const unsigned pages = (count + page - 1) / page;
    unsigned cur = 0;
    while(1) {
        const unsigned start = cur * page;
        const unsigned len = (count - start > page) ? page : (count - start);
        print_block(addr + start, len);

        if (pages == 1) return;
        kout << "-- " << (cur + 1) << '/' << pages << "  enter=next  up=prev  q=quit\n";

        switch (io.page_key()) {
        case PageKey::Quit: return;
        case PageKey::Next: if (cur + 1 < pages) cur++; break;
        case PageKey::Prev: if (cur > 0) cur--; break;
        default: break;
        }
    }
}

void ShellVM::push(u32 v) {
    if (sp < stack_max) stack[sp++] = v;
}

u32 ShellVM::pop() {
    if (sp == 0) return 0;   // 前端按 argc 编译，正常到不了这里
    return stack[--sp];
}

void ShellVM::exec(const Insn &in) {
    switch (in.op) {
    case Op::Push: push(in.imm); break;
    case Op::Help: cmd_help(); break;
    case Op::Obj:  cmd_obj();  break;
    case Op::Fs:   cmd_fs();   break;
    case Op::Read: cmd_read(); break;
    case Op::Mem:  cmd_mem();  break;
    default:
        kout << "bad opcode: " << (unsigned)in.op << '\n';
        break;
    }
}

void ShellVM::run(const ShellNode &node) {
    if (!node.ok) {
        kout << node.error << '\n';
        return;
    }

    sp = 0;
    for (auto &in : node.code) exec(in);
    sp = 0;
}

void ShellVM::cmd_help() {
    unsigned n = 0;
    const OpInfo *t = op_table(n);
    for (unsigned i = 0; i < n; i++) {
        kout << t[i].name;
        for (unsigned a = 0; a < t[i].argc; a++) {
            kout << "  " << t[i].args[a].name << '|';
            kout.hex((unsigned char)t[i].args[a].def);   // 缺省值，两位十六进制
        }
        kout << "  -- " << t[i].usage << '\n';
    }
    kout << "args: name|value, or positional in table order; values are hex\n";
    kout << "pages: enter/down=next, up/b=prev, q=quit\n";
}

void ShellVM::cmd_obj() {
    if (!kom) { kout << "no kom\n"; return; }
    kom->visit(dump_obj, nullptr);
}

void ShellVM::cmd_fs() {
    if (!driverletterspace) { kout << "no dls\n"; return; }
    if (driverletterspace->space.get_size() == 0) { kout << "no fs\n"; return; }
    for (auto &d : driverletterspace->space)
        kout << "[fs] " << d.driver_letter << ":  cid=" << d.cid << '\n';
}

// read <disk#> <lba> [count]：压栈顺序 disk, lba, count，所以反着弹。
void ShellVM::cmd_read() {
    const unsigned cnt = pop();
    const unsigned lba = pop();
    const unsigned idx = pop();

    DiskPick pick;
    pick.want = idx;
    kom->visit(pick_disk, &pick);
    if (!pick.disk) { kout << "no such disk\n"; return; }

    static u8 buf[8 * 512];
    unsigned got = pick.disk->read(buf, lba, 0, cnt);
    kout << "[read] disk" << idx << " lba=" << lba << " -> " << got << " bytes\n";
    if (got == (unsigned)-1 || got == 0) return;
    if (got > sizeof buf) got = sizeof buf;
    dump_bytes((unsigned)buf, got);
}

// mem <addr> [count]：压栈顺序 addr, count。
void ShellVM::cmd_mem() {
    const unsigned cnt = pop();
    const unsigned addr = pop();
    dump_bytes(addr, cnt);
}
