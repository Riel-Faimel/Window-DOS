#ifndef __kernel_basicshell_bs_hpp
#define __kernel_basicshell_bs_hpp

#include <global/type.hpp>
#include <global/WinHandle.hpp>
#include <lib/string>
#include <HL/container>

#include <interface/svm/op.hpp>
#include <interface/svm/data.hpp>
#include <svm/svm.hpp>

class BasicShell {
public:
    // 参数种类：编译器据此决定是直接压整数，还是"压整数再 Open 成引用"。
    enum class ArgKind : u8 {
        Int = 0,
        Disk,
    };

private:
    enum class PageKey {
        Next,
        Prev,
        Quit,
        Ignore,
    };

    struct OpArg {
        const char *name;   // 命名参数用，也用于报错
        u32 def;
        u32 lo;
        u32 hi;             // hi == 0 表示不检查上限
        ArgKind kind = ArgKind::Int;
    };

public:
    struct OpInfo {
        svm::Op          op;
        const char *name;
        const char *usage;
        u8          argc;
        bool        out;     // 需要输出：编译时会多压一个缓冲区指针操作数
        OpArg       args[3];
    };
    
private:
    static constexpr unsigned line_max = 128;

    WinHandle win;          // 打开 kbd 的句柄

    // 执行状态：由 sysvm->create_thread() 分配，本对象持有。
    svm::SysVMThread *thread = nullptr;

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
                 const OpInfo &info, u32 out_buf);

    void print_help();
    void print_block(const u8 *p, unsigned base_addr, unsigned len);
    void dump_bytes(const u8 *p, unsigned base_addr, unsigned len);
    void dump_recs(const svm::OutBuf &buf);

    // ---- 脚本语法：变量、条件、循环 ----
    struct Var {
        String name;
        u32    slot = 0;
        svm::Value::TypeID type = svm::Value::TypeID::Nil;
    };
    Var vars[12];
    unsigned var_n = 0;

    bool compile_script(svm::ShellNode &re, const String &text, u32 out_buf);
    bool compile_if(svm::ShellNode &re, const String &text, u32 out_buf);
    bool compile_while(svm::ShellNode &re, const String &text, u32 out_buf);
    bool emit_assign(svm::ShellNode &re, const String &text);
    bool emit_command(svm::ShellNode &re, const String &text, u32 out_buf);
    bool emit_operand(svm::ShellNode &re, const String &word);
    bool parse_cond(const String &cond, String &left, String &op, String &right);
    void emit_cond_jumps(svm::ShellNode &re, const String &op, bool jump_true);
    bool emit_cond(svm::ShellNode &re, const String &cond, bool jump_true);
    bool emit_stmt(svm::ShellNode &re, const String &text, u32 out_buf);
    Var *find_var(const String &name);

public:
    BasicShell();

    /** 有没有打开到输入流（没有的话 read_line 会一直等）。 */
    bool ready() const { return win.extra != nullptr; }

    /**
     * 阻塞读一行并编译。空行返回 false。
     * out_buf 是输出缓冲区的地址，需要输出的命令会把它作为第一个操作数压栈。
     */
    bool read(svm::ShellNode &out, u32 out_buf = 0);

    // 分页等键：enter/空格/↓ 下一页，↑/b/退格 上一页，q 退出。
    PageKey page_key();

    [[noreturn]] void main();
};

#endif