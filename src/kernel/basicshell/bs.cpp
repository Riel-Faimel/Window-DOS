#include "_bs.hpp"

using namespace svm;

BasicShell::BasicShell(): win{} {
    if (sdm) sdm->open(win, String("kbd"));
}

char BasicShell::getch() {
    while (keypos >= keylen) {
        const unsigned n = sdm ? sdm->read(win, keybuf, sizeof keybuf) : 0;
        if (n == 0) { asm volatile ("hlt"); continue; }
        keylen = n;
        keypos = 0;
    }
    return keybuf[keypos++];
}

char BasicShell::eat_escape() {
    char c = getch();
    if (c != '[') return c;
    c = getch();
    while (c >= '0' && c <= '9') c = getch();
    return c;
}

void BasicShell::read_line() {
    line.clear();
    for (;;) {
        char c = getch();
        if (c == '\n') { kout << '\n'; return; }
        if (c == '\b') {
            if (!line.empty()) {
                line = line.substr(0, line.length() - 1);
                kout << '\b';
            }
            continue;
        }
        if (c == 0x1B) { eat_escape(); continue; }   // 方向键：吃掉，别让输入卡住
        if (c < 0x20 || c > 0x7E) continue;
        if (line.length() + 1 < line_max) {
            line += c;          // 不能用 append(char)：String(unsigned) 是"数字转十六进制"
            kout << c;
        }
    }
}

void BasicShell::compile(ShellNode &re, const rtl::array<String> &words,
                         const OpInfo &info, u32 out_buf) {
    u32 val[3] = {0, 0, 0};
    bool set[3] = {false, false, false};
    unsigned next_pos = 0;

    for (unsigned w = 1; w < words.get_size(); w++) {
        const String &word = words[w];
        const unsigned bar = word.find("|");

        // 位置参数：填下一个还没被占的槽。
        if (bar == (unsigned)-1) {
            while (next_pos < info.argc && set[next_pos]) next_pos++;
            if (next_pos >= info.argc) {
                re.ok = false;
                re.error = String("too many arguments, usage: ") + info.usage;
                return;
            }
            if (!parse_hex(word, val[next_pos])) {
                re.ok = false;
                re.error = String("bad number: ") + word;
                return;
            }
            set[next_pos] = true;
            next_pos++;
            continue;
        }

        // 命名参数：name|value。
        const String name = word.substr(0, static_cast<int>(bar));
        const String text = word.substr(static_cast<int>(bar) + 1);

        unsigned idx = info.argc;
        for (unsigned i = 0; i < info.argc; i++)
            if (name == info.args[i].name) { idx = i; break; }

        if (idx == info.argc) {
            re.ok = false;
            re.error = String("unknown option: ") + name;
            return;
        }
        if (set[idx]) {
            re.ok = false;
            re.error = String("duplicate option: ") + name;
            return;
        }
        if (!parse_hex(text, val[idx])) {
            re.ok = false;
            re.error = String("bad number: ") + text;
            return;
        }
        set[idx] = true;
    }

    // 需要输出的命令：输出缓冲区指针是第一个操作数，所以最先压。
    if (info.out) re.code.append(Insn{Op::Push, out_buf});

    // 补缺省值、查范围，然后按表里的顺序压栈。
    for (unsigned i = 0; i < info.argc; i++) {
        const OpArg &desc = info.args[i];
        if (!set[i]) val[i] = desc.def;
        if (desc.hi && (val[i] < desc.lo || val[i] > desc.hi)) {
            re.ok = false;
            re.error = String("out of range: ") + desc.name;
            return;
        }
        re.code.append(Insn{Op::Push, val[i]});
    }

    re.code.append(Insn{info.op, 0});
}

bool BasicShell::read(ShellNode &re, u32 out_buf) {
    read_line();

    re.code.clear();
    re.ok = true;
    re.error = String();

    auto words = split_args(line);
    if (words.get_size() == 0) return false;

    const OpInfo *info = op_lookup(words[0]);
    if (!info) {
        re.ok = false;
        re.error = String("unknown command: ") + words[0];
        return true;
    }

    compile(re, words, *info, out_buf);
    return true;
}

PageKey BasicShell::page_key() {
    for (;;) {
        char c = getch();
        if (c == '\n' || c == ' ') return PageKey::Next;
        if (c == 'q' || c == 'Q') return PageKey::Quit;
        if (c == 'b' || c == 'B' || c == '\b') return PageKey::Prev;
        if (c == 0x1B) {
            switch (eat_escape()) {
            case 'B': case '6': return PageKey::Next;   // Down / PageDown
            case 'A': case '5': return PageKey::Prev;   // Up   / PageUp
            default: return PageKey::Ignore;
            }
        }
    }
}

void BasicShell::print_help() {
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

void BasicShell::print_block(const u8 *p, unsigned base_addr, unsigned len) {
    for (unsigned at = 0; at < len; at += 16) {
        kout << "0x"; kout.hex(base_addr + at); kout << ": ";
        for (unsigned i = 0; i < 16; i++) {
            if (at + i < len) { kout.hex(p[at + i]); kout << ' '; }
            else kout << "   ";
        }
        kout << '|';
        for (unsigned i = 0; i < 16 && at + i < len; i++) {
            char ch = char(p[at + i]);
            kout << ((ch >= 0x20 && ch < 0x7F) ? ch : '.');
        }
        kout << "|\n";
    }
}

// 一页 = 16 行 × 16 字节；多于一页就分页。
void BasicShell::dump_bytes(const u8 *p, unsigned base_addr, unsigned len) {
    constexpr unsigned page = 16 * 16;
    if (len == 0) return;

    const unsigned pages = (len + page - 1) / page;
    unsigned cur = 0;
    for (;;) {
        const unsigned start = cur * page;
        const unsigned n = (len - start > page) ? page : (len - start);
        print_block(p + start, base_addr + start, n);

        if (pages == 1) return;
        kout << "    " << (cur + 1) << '/' << pages << "  enter=next  up=prev  q=quit\n";

        switch (page_key()) {
        case PageKey::Quit: return;
        case PageKey::Next: if (cur + 1 < pages) cur++; break;
        case PageKey::Prev: if (cur > 0) cur--; break;
        default: break;
        }
    }
}

// obj / fs 的记录流：VM 只给结构化数据，文案在这里。
void BasicShell::dump_recs(const OutBuf &buf) {
    const unsigned n = buf.len / sizeof(ObjRec);
    auto *recs = reinterpret_cast<const ObjRec *>(buf.data);

    for (unsigned i = 0; i < n; i++) {
        switch (recs[i].kind) {
        case RecKind::Phys:
            kout << "[obj] phys 0x"; kout.hex(recs[i].a); kout << '\n';
            break;
        case RecKind::Part:
            kout << "[obj] part lba=" << recs[i].a
                 << " sectors=" << recs[i].b << " type=0x";
            kout.hex(recs[i].c);
            kout << '\n';
            break;
        case RecKind::Serial: kout << "[obj] serial\n"; break;
        case RecKind::Frame:  kout << "[obj] frame\n";  break;
        case RecKind::Drive:
            kout << "[fs] " << char(recs[i].a) << ":  cid=" << recs[i].b << '\n';
            break;
        default: kout << "[obj] unknown\n"; break;
        }
    }
}

[[noreturn]]
void BasicShell::main() {
    if (!ready()) kout << "[shell] no input stream, need \"kbd\"\n";

    if (!sysvm) {
        kout << "[shell] no SysVM in the kernel\n";
        for (;;) asm volatile ("hlt");
    }

    static u8 storage[4096];
    out.data = storage;
    out.cap = sizeof storage;

    for (;;) {
        kout << "dbg> ";

        out.reset();

        ShellNode node;
        if (!read(node, reinterpret_cast<u32>(&out))) continue;
        if (!node.ok) { kout << node.error << '\n'; continue; }
        if (node.code.get_size() == 0) continue;

        const Op op = node.code[node.code.get_size() - 1].op;
        if (op == Op::Help) { print_help(); continue; }

        sysvm->run(node);

        switch (op) {
        case Op::Read: {
            // 参数是编译期常量，前端自己知道，不用 VM 回报。
            const unsigned disk = node.code[1].imm;   // [0] 是输出缓冲区指针
            const unsigned lba  = node.code[2].imm;
            kout << "[read] disk" << disk << " lba=" << lba
                 << " -> " << out.len << " bytes\n";
            dump_bytes(out.data, 0, out.len);
            break;
        }
        case Op::Mem:
            dump_bytes(out.data, node.code[1].imm, out.len);
            break;
        case Op::Obj:
        case Op::Fs:
            dump_recs(out);
            break;
        default:
            break;
        }
    }
}
