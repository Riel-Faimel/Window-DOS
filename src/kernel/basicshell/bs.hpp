#ifndef __kernel_basicshell_bs_hpp
#define __kernel_basicshell_bs_hpp

#include <global/type.hpp>
#include <global/WinHandle.hpp>
#include <lib/string>
#include <HL/container>
#include <interface/svm/op.hpp>

/**
 * 调试环境的前端，也是它的入口。
 *
 *   - 键盘输入与行编辑：从 SDM 按名字打开流服务（"kbd"），只认“从窗口句柄读字节”
 *   - 词法与语法树：按命令表把一行编译成指令序列（需要输出的命令多压一个缓冲区指针）
 *   - 分页按键：分页是显示的一部分，属于前端
 *
 * 执行交给内核持有的 SysVM；它只吃指令、只写原始字节，怎么显示在这里决定。
 */
enum class PageKey {
    Next,
    Prev,
    Quit,
    Ignore,
};

class BasicShell {
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
    // 需要输出的命令，先压一个输出缓冲区指针（操作数），再压参数。
    void compile(svm::ShellNode &out, const rtl::array<String> &words,
                 const svm::OpInfo &info, u32 out_buf);

    void print_help();
    void print_block(const u8 *p, unsigned base_addr, unsigned len);
    void dump_bytes(const u8 *p, unsigned base_addr, unsigned len);
    void dump_recs(const svm::OutBuf &buf);

public:
    BasicShell();

    /** 有没有打开到输入流（没有的话 read_line 会一直等）。 */
    bool ready() const { return win.extra != nullptr; }

    /**
     * 阻塞读一行并编译。空行返回 false。
     * out_buf 是输出缓冲区的地址，需要输出的命令会把它作为第一个操作数压栈。
     */
    bool read(svm::ShellNode &out, u32 out_buf = 0);

    /**
     * 分页等键：enter/空格/↓ 下一页，↑/b/退格 上一页，q 退出。
     */
    PageKey page_key();

    [[noreturn]] void main();
};

#endif