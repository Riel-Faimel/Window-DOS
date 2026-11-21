#include "shell_in.hpp"
#include <stdio>
#include <SrvDevMgr/SDM.hpp>

namespace {

// 按空格拆参，丢掉空词（连续空格会拆出空串）。
rtl::array<String> split_args(const String &line) {
    rtl::array<String> argv;
    for (auto &w : line.split(' '))
        if (!w.empty()) argv.append(rtl::move(w));
    return argv;
}

// 严格十六进制解析：整个词都必须是数字，不留"读到非法字符就停"的洞。
bool parse_hex(const String &w, u32 &out) {
    if (w.empty()) return false;
    u32 v = 0;
    for (char c : w) {
        unsigned d;
        if (c >= '0' && c <= '9') d = unsigned(c - '0');
        else if (c >= 'a' && c <= 'f') d = unsigned(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') d = unsigned(c - 'A' + 10);
        else return false;
        v = (v << 4) | d;
    }
    out = v;
    return true;
}

} // namespace

ShellIn::ShellIn(): win{} {
    if (sdm) sdm->open(win, String("kbd"));
}

char ShellIn::getch() {
    while (keypos >= keylen) {
        const unsigned n = sdm ? sdm->read(win, keybuf, sizeof keybuf) : 0;
        if (n == 0) { asm volatile ("hlt"); continue; }
        keylen = n;
        keypos = 0;
    }
    return keybuf[keypos++];
}

char ShellIn::eat_escape() {
    char c = getch();
    if (c != '[') return c;
    c = getch();
    while (c >= '0' && c <= '9') c = getch();
    return c;
}

void ShellIn::read_line() {
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

void ShellIn::compile(ShellNode &out, const rtl::array<String> &words, const OpInfo &info) {
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
                out.ok = false;
                out.error = String("too many arguments, usage: ") + info.usage;
                return;
            }
            if (!parse_hex(word, val[next_pos])) {
                out.ok = false;
                out.error = String("bad number: ") + word;
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
            out.ok = false;
            out.error = String("unknown option: ") + name;
            return;
        }
        if (set[idx]) {
            out.ok = false;
            out.error = String("duplicate option: ") + name;
            return;
        }
        if (!parse_hex(text, val[idx])) {
            out.ok = false;
            out.error = String("bad number: ") + text;
            return;
        }
        set[idx] = true;
    }

    // 补缺省值、查范围，然后按表里的顺序压栈。
    for (unsigned i = 0; i < info.argc; i++) {
        const OpArg &desc = info.args[i];
        if (!set[i]) val[i] = desc.def;
        if (desc.hi && (val[i] < desc.lo || val[i] > desc.hi)) {
            out.ok = false;
            out.error = String("out of range: ") + desc.name;
            return;
        }
        out.code.append(Insn{Op::Push, val[i]});
    }

    out.code.append(Insn{info.op, 0});
}

bool ShellIn::read(ShellNode &out) {
    read_line();

    out.code.clear();
    out.ok = true;
    out.error = String();

    auto words = split_args(line);
    if (words.get_size() == 0) return false;

    const OpInfo *info = op_lookup(words[0]);
    if (!info) {
        out.ok = false;
        out.error = String("unknown command: ") + words[0];
        return true;
    }

    compile(out, words, *info);
    return true;
}

PageKey ShellIn::page_key() {
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
