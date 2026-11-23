#pragma GCC optimize("Os")
#include "_bs.hpp"

using namespace svm;

namespace {

// Frontend-only module names. SVM only sees the numeric selector.
enum class StatusModule : u32 {
    All = 0,
    Mm,
    Page,
    Obj,
    Fs,
};

bool status_module_from_name(const String &name, StatusModule &out) {
    if (name == String("all"))   { out = StatusModule::All;  return true; }
    if (name == String("mm") ||
        name == String("heap") ||
        name == String("memory")){ out = StatusModule::Mm;   return true; }
    if (name == String("page") ||
        name == String("paging")){ out = StatusModule::Page; return true; }
    if (name == String("obj") ||
        name == String("objects")){ out = StatusModule::Obj; return true; }
    if (name == String("fs") ||
        name == String("drives")){ out = StatusModule::Fs;   return true; }

    u32 v;
    if (parse_hex(name, v) && v <= static_cast<u32>(StatusModule::Fs)) {
        out = static_cast<StatusModule>(v);
        return true;
    }
    return false;
}

} // namespace

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
    while(1) {
        char c = getch();
        if (c == '\n') {
            // 行尾的 `\` 是续行：换成语句分隔，接着读下一行。
            if (!line.empty() && line.c_str()[line.length() - 1] == '\\') {
                line = line.substr(0, line.length() - 1) + String(" ; ");
                kout << '\n';
                continue;
            }
            kout << '\n';
            return;
        }
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

// ======================= 命令参数（老路径） =======================

void BasicShell::compile(ShellNode &re, const rtl::array<String> &words,
                         const OpInfo &info) {
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

    // 补缺省值、查范围，然后按表里的顺序压栈。
    for (unsigned i = 0; i < info.argc; i++) {
        const OpArg &desc = info.args[i];
        if (!set[i]) val[i] = desc.def;
        if (desc.hi && (val[i] < desc.lo || val[i] > desc.hi)) {
            re.ok = false;
            re.error = String("out of range: ") + desc.name;
            return;
        }
        // 需要引用的参数：先压整数，Open 把对象放进编译器的槽，
        // 再把那个 Ref 压给命令本身。
        if (desc.kind == ArgKind::Disk) {
            re.code.append(Insn{Op::Push, Value{Value::TypeID::Int, val[i]}});
            re.code.append(Insn{Op::Open, Value{Value::TypeID::Ref, disk_slot}});
            re.code.append(Insn{Op::Push, Value{Value::TypeID::Ref, disk_slot}});
        } else {
            re.code.append(Insn{Op::Push, Value{Value::TypeID::Int, val[i]}});
        }
    }

    re.code.append(Insn{info.op, Value{}});
}

// ======================= 脚本语法 =======================

BasicShell::Var *BasicShell::find_var(const String &name) {
    for (unsigned i = 0; i < var_n; i++)
        if (vars[i].name == name) return &vars[i];
    return nullptr;
}

// 压一个操作数：十六进制字面量按 Int，变量名按 Ref（Cmp 等会解引用）。
bool BasicShell::emit_operand(ShellNode &re, const String &word) {
    u32 v;
    if (parse_hex(word, v)) {
        re.code.append(Insn{Op::Push, Value{Value::TypeID::Int, v}});
        return true;
    }

    Var *var = find_var(word);
    if (!var) {
        re.ok = false;
        re.error = String("unknown variable: ") + word;
        return false;
    }
    re.code.append(Insn{Op::Push, Value{Value::TypeID::Ref, var->slot}});
    return true;
}

// "a OP b"，OP ∈ {<, ==, >}
bool BasicShell::parse_cond(const String &cond, String &left, String &op, String &right) {
    const String c = cond.trim();

    if (c.length() >= 2) {
        const unsigned eq = c.find("==");
        if (eq != (unsigned)-1) {
            left  = c.substr(0, static_cast<int>(eq)).trim();
            right = c.substr(static_cast<int>(eq) + 2,
                             static_cast<int>(c.length() - eq - 2)).trim();
            op = String("==");
            return !left.empty() && !right.empty();
        }
    }

    const unsigned lt = c.find("<");
    if (lt != (unsigned)-1) {
        left  = c.substr(0, static_cast<int>(lt)).trim();
        right = c.substr(static_cast<int>(lt) + 1,
                         static_cast<int>(c.length() - lt - 1)).trim();
        op = String("<");
        return !left.empty() && !right.empty();
    }

    const unsigned gt = c.find(">");
    if (gt != (unsigned)-1) {
        left  = c.substr(0, static_cast<int>(gt)).trim();
        right = c.substr(static_cast<int>(gt) + 1,
                         static_cast<int>(c.length() - gt - 1)).trim();
        op = String(">");
        return !left.empty() && !right.empty();
    }

    return false;
}

// 条件结果（≤> 的 -1/0/1）里，要比哪几个值才跳。
// Jc 不弹栈，所以可以连发多条，共用同一份「目标 + Ordering」。
void BasicShell::emit_cond_jumps(ShellNode &re, const String &op, bool jump_true) {
    const s32 t = (op == String("<")) ? -1 : (op == String("==")) ? 0 : 1;

    for (s32 v = -1; v <= 1; v++) {
        const bool hit = jump_true ? (v == t) : (v != t);
        if (hit) re.code.append(Insn{Op::Jc, Value{Value::TypeID::Ordering, static_cast<u32>(v)}});
    }
}

// 条件：栈上必须已经压好「目标」，这里压左操作数、Cmp 右操作数、然后 Jc。
bool BasicShell::emit_cond(ShellNode &re, const String &cond, bool jump_true) {
    String left, op, right;
    if (!parse_cond(cond, left, op, right)) {
        re.ok = false;
        re.error = String("bad condition: ") + cond;
        return false;
    }

    if (!emit_operand(re, left)) return false;

    u32 v;
    if (parse_hex(right, v)) {
        re.code.append(Insn{Op::Cmp, Value{Value::TypeID::Int, v}});
    } else {
        Var *var = find_var(right);
        if (!var) {
            re.ok = false;
            re.error = String("unknown variable: ") + right;
            return false;
        }
        re.code.append(Insn{Op::Cmp, Value{Value::TypeID::Ref, var->slot}});
    }

    emit_cond_jumps(re, op, jump_true);
    return true;
}

// Type name = value
bool BasicShell::emit_assign(ShellNode &re, const String &text) {
    auto words = split_args(text);
    if (words.get_size() < 4 || words[2] != String("=")) {
        re.ok = false;
        re.error = String("bad assignment: ") + text;
        return false;
    }

    const String &tn = words[0];
    Value::TypeID type = Value::TypeID::Nil;
    if (tn == String("Int"))      type = Value::TypeID::Int;
    else if (tn == String("Obj")) type = Value::TypeID::Obj;
    else if (tn == String("Ref")) type = Value::TypeID::Ref;
    else {
        re.ok = false;
        re.error = String("unknown type: ") + tn;
        return false;
    }

    const String &name = words[1];

    Var *var = find_var(name);
    if (!var) {
        if (var_n >= vars_max) {
            re.ok = false;
            re.error = String("too many variables");
            return false;
        }
        var = &vars[var_n];
        var->name = name;
        var->slot = var_base + var_n;
        var->type = type;
        var_n++;
    } else if (var->type != type) {
        re.ok = false;
        re.error = String("type mismatch for ") + name;
        return false;
    }

    // Obj 变量：右边是盘号，走 Open 解析成内核对象指针存进这个变量。
    if (type == Value::TypeID::Obj) {
        u32 disk;
        if (!parse_hex(words[3], disk)) {
            re.ok = false;
            re.error = String("bad disk number: ") + words[3];
            return false;
        }
        re.code.append(Insn{Op::Push, Value{Value::TypeID::Int, disk}});
        re.code.append(Insn{Op::Open, Value{Value::TypeID::Ref, var->slot}});
        return true;
    }

    if (!emit_operand(re, words[3])) return false;
    re.code.append(Insn{Op::Assign, Value{Value::TypeID::Ref, var->slot}});
    return true;
}

bool BasicShell::emit_command(ShellNode &re, const String &text) {
    auto words = split_args(text);
    if (words.get_size() == 0) return true;

    const OpInfo *info = op_lookup(words[0]);
    if (!info) {
        re.ok = false;
        re.error = String("unknown command: ") + words[0];
        return false;
    }

    // Status has an optional module name, which is not a numeric argument.
    if (info->op == Op::Status) {
        if (words.get_size() > 2) {
            re.ok = false;
            re.error = String("too many arguments, usage: ") + info->usage;
            return false;
        }

        StatusModule module = StatusModule::All;
        if (words.get_size() == 2 &&
            !status_module_from_name(words[1], module)) {
            re.ok = false;
            re.error = String("unknown status module: ") + words[1];
            return false;
        }

        re.code.append(Insn{
            Op::Push,
            Value{Value::TypeID::Int, static_cast<u32>(module)}
        });
        re.code.append(Insn{Op::Status, Value{}});
        return true;
    }

    compile(re, words, *info);
    if (!re.ok) return false;

    // 输出指令的结果指针要留给 shell；其它命令的返回值不留在栈上。
    const bool output =
        info->op == Op::Read || info->op == Op::Mem ||
        info->op == Op::Obj || info->op == Op::Fs ||
        info->op == Op::Status;
    if (!output)
        re.code.append(Insn{Op::Drop, Value{}});
    return true;
}

bool BasicShell::emit_stmt(ShellNode &re, const String &text) {
    const String t = text.trim();
    if (t.empty()) return true;

    auto words = split_args(t);
    if (words.get_size() >= 2) {
        const String &tn = words[0];
        if ((tn == String("Int") || tn == String("Obj") ||
             tn == String("Ref")) &&
            t.find("=") != (unsigned)-1)
            return emit_assign(re, t);
    }

    return emit_command(re, t);
}

// if (cond) stmt ; stmt ...
bool BasicShell::compile_if(ShellNode &re, const String &text) {
    const unsigned close = text.find(")");
    if (close == (unsigned)-1 || close < 4) {
        re.ok = false;
        re.error = String("bad if, usage: if(cond) stmt");
        return false;
    }

    const String cond = text.substr(3, static_cast<int>(close - 3));
    const String body = text.substr(static_cast<int>(close) + 1,
                                    static_cast<int>(text.length() - close - 1));

    const u32 tgt = re.code.get_size();
    re.code.append(Insn{Op::Push, Value{Value::TypeID::Int, 0}});   // 目标，稍后回填

    if (!emit_cond(re, cond, false)) return false;                  // 条件为假 → 跳过 body

    for (auto &stmt : body.split(';'))
        if (!emit_stmt(re, stmt)) return false;

    const u32 end = re.code.get_size();
    re.code[tgt].imm.value = end;

    re.code.append(Insn{Op::Drop, Value{}});   // Ordering
    re.code.append(Insn{Op::Drop, Value{}});   // 目标
    return true;
}

// while (cond) stmt ; stmt ...
bool BasicShell::compile_while(ShellNode &re, const String &text) {
    const unsigned close = text.find(")");
    if (close == (unsigned)-1 || close < 7) {
        re.ok = false;
        re.error = String("bad while, usage: while(cond) stmt");
        return false;
    }

    const String cond = text.substr(6, static_cast<int>(close - 6));
    const String body = text.substr(static_cast<int>(close) + 1,
                                    static_cast<int>(text.length() - close - 1));

    const u32 top = re.code.get_size();

    const u32 tgt = re.code.get_size();
    re.code.append(Insn{Op::Push, Value{Value::TypeID::Int, 0}});   // 目标，稍后回填

    if (!emit_cond(re, cond, false)) return false;                  // 条件为假 → 退出循环

    for (auto &stmt : body.split(';'))
        if (!emit_stmt(re, stmt)) return false;

    re.code.append(Insn{Op::Drop, Value{}});   // Ordering
    re.code.append(Insn{Op::Drop, Value{}});   // 目标
    re.code.append(Insn{Op::Jmp, Value{Value::TypeID::Int, top}});

    const u32 end = re.code.get_size();
    re.code[tgt].imm.value = end;

    re.code.append(Insn{Op::Drop, Value{}});   // Ordering
    re.code.append(Insn{Op::Drop, Value{}});   // 目标
    return true;
}

bool BasicShell::compile_script(ShellNode &re, const String &text) {
    if (text.length() > 6 && text.substr(0, 6) == String("while("))
        return compile_while(re, text);
    if (text.length() > 3 && text.substr(0, 3) == String("if("))
        return compile_if(re, text);
    return emit_stmt(re, text);
}

bool BasicShell::read(ShellNode &re) {
    read_line();

    re.code.clear();
    re.ok = true;
    re.error = String();

    const String t = line.trim();
    if (t.empty()) return false;

    // help 是纯前端的东西（它打印命令表），不编译成指令：留一个空程序给 main 识别。
    auto words = split_args(t);
    if (words.get_size() > 0 && words[0] == String("help")) return true;

    compile_script(re, t);
    return true;
}

BasicShell::PageKey BasicShell::page_key() {
    while(1) {
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
    kout << "help  -- this list\n";

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
    kout << "vars:  Int x = 5   (type name = value)\n";
    kout << "flow:  if(a < 5) cmd ; cmd \\   while(a > 0) cmd ; cmd \\\n";
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
    while(1) {
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

void BasicShell::dump_read(const ReadResult &res) {
    if (res.len)
        dump_bytes(res.addr, 0, static_cast<unsigned>(res.len));
}

void BasicShell::dump_mem(const MemResult &res) {
    if (res.len)
        dump_bytes(res.addr, 0, static_cast<unsigned>(res.len));
}

void BasicShell::dump_obj_entries(const ObjEntry *addr, size_t len) {
    for (size_t i = 0; i < len; i++) {
        const ObjEntry &rec = addr[i];
        switch (rec.kind) {
        case ObjKind::Phys:
            kout << "[obj] phys 0x"; kout.hex(rec.a); kout << '\n';
            break;
        case ObjKind::Part:
            kout << "[obj] part lba=" << rec.a
                 << " sectors=" << rec.b << " type="
                 << (rec.c) << '\n';
            break;
        case ObjKind::Serial: kout << "[obj] serial\n"; break;
        case ObjKind::Frame:  kout << "[obj] frame\n";  break;
        default: kout << "[obj] unknown\n"; break;
        }
    }
}

void BasicShell::dump_fs_entries(const DriveEntry *addr, size_t len) {
    for (size_t i = 0; i < len; i++) {
        const DriveEntry &d = addr[i];
        kout << "[fs] " << char(d.letter) << ":  cid=" << d.cid << '\n';
    }
}

// obj / fs are still kept as internal result types, but user commands go
// through Status. These wrappers keep the VM result switch simple.
void BasicShell::dump_obj(const ObjResult &res) {
    dump_obj_entries(res.addr, res.len);
}

void BasicShell::dump_fs(const FsResult &res) {
    dump_fs_entries(res.addr, res.len);
}

void BasicShell::dump_status(const StatusResult &res) {
    const auto selector = static_cast<StatusModule>(res.selector);
    const bool all = selector == StatusModule::All;

    if (all || selector == StatusModule::Mm) {
        kout << "[mm] total=" << (u32)res.total_mem
             << "\n alloced=" << (u32)res.alloced_mem
             << "\n used=" << (u32)res.used_mem
             << "\n free=" << (u32)res.free_mem << '\n';
    }

    if (all || selector == StatusModule::Page) {
        kout << "[page] total=" << (u32)res.page_total
             << "\n free=" << (u32)res.page_free << '\n';
    }

    if (all || selector == StatusModule::Obj)
        dump_obj_entries(res.objs, res.obj_len);

    if (all || selector == StatusModule::Fs)
        dump_fs_entries(res.drives, res.fs_len);
}

[[noreturn]]
void BasicShell::main() {
    if (!ready()) kout << "[shell] no input stream, need \"kbd\"\n";

    if (!sysvm) {
        kout << "[shell] no SysVM in the kernel\n";
        while(1) asm volatile ("hlt");
    }

    thread = sysvm->create_thread();

    while(1) {
        // 上一条命令的结果归线程所有，先释放再读新命令。
        thread->reset();

        kout << "dbg> ";
        ShellNode node;
        if (!read(node)) continue;
        if (!node.ok) { kout << node.error << '\n'; continue; }

        // help 不产生指令（见 read()），空程序就是它。
        if (node.code.get_size() == 0) { print_help(); continue; }

        if (!sysvm->run(*thread, node)) continue;

        const Value result = thread->pop();
        switch (result.type) {
        case Value::TypeID::ReadOut:
            dump_read(*reinterpret_cast<const ReadResult *>(result.value));
            break;
        case Value::TypeID::MemOut:
            dump_mem(*reinterpret_cast<const MemResult *>(result.value));
            break;
        case Value::TypeID::ObjOut:
            dump_obj(*reinterpret_cast<const ObjResult *>(result.value));
            break;
        case Value::TypeID::FsOut:
            dump_fs(*reinterpret_cast<const FsResult *>(result.value));
            break;
        case Value::TypeID::StatusOut:
            dump_status(*reinterpret_cast<const StatusResult *>(result.value));
            break;
        default:
            break;
        }
    }
}
