#ifndef __kernel_basicshell_shell_in_hpp
#define __kernel_basicshell_shell_in_hpp

#include <global/type.hpp>
#include <global/WinHandle.hpp>
#include <lib/string>
#include <HL/container>
#include "op.hpp"

/**
 * 一个命令编译出来的东西：一段指令（若干 push，最后一条命令指令）。
 * ok=false 时 code 为空，error 由执行器打印。
 */
struct ShellNode {
    rtl::array<Insn> code;
    bool   ok = true;
    String error;
};

enum class PageKey {
    Next,
    Prev,
    Quit,
    Ignore,
};

/**
 * 前端：键盘输入、行编辑、词法与语法树。
 *
 * 输入不是直接拿键盘驱动，而是从 SDM 按名字打开一个流服务（"kbd"）；
 * 这里只认"从窗口句柄读字节"这件事。
 */
class ShellIn {
    static constexpr unsigned line_max = 128;

    WinHandle win;          // 打开 kbd 的句柄

    String line;

    char keybuf[8];
    unsigned keylen = 0;
    unsigned keypos = 0;

    char getch();
    char eat_escape();
    void read_line();

    // 按签名表把参数编译成 push 指令，缺省值和范围检查都在这里。
    void compile(ShellNode &out, const rtl::array<String> &words, const OpInfo &info);

public:
    ShellIn();

    /** 有没有打开到输入流（没有的话 read_line 会一直等）。 */
    bool ready() const { return win.extra != nullptr; }

    /**
     * 阻塞读一行并编译。空行返回 false。
     */
    bool read(ShellNode &out);

    /**
     * 分页等键：enter/空格/↓ 下一页，↑/b/退格 上一页，q 退出。
     */
    PageKey page_key();
};

#endif
