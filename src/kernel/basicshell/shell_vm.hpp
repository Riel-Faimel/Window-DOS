#ifndef __kernel_basicshell_shell_vm_hpp
#define __kernel_basicshell_shell_vm_hpp

#include <global/type.hpp>
#include <HL/container>
#include "op.hpp"

struct ShellNode;
class ShellIn;

/**
 * 执行器：一个值栈 + 指令循环。
 *
 * 参数不从文字里取，而是前端按签名表编译成 push 指令，命令只管从栈上弹。
 * 现在栈上只有整数对象，以后 Value 变成 { kind, payload } 时指令集不用动。
 */
class ShellVM {
    ShellIn &io;

    // 值栈：定长，压/弹就是 sp 加减。现在栈上只有整数对象。
    static constexpr unsigned stack_max = 64;
    u32 stack[stack_max];
    unsigned sp = 0;

    void exec(const Insn &in);
    void push(u32 v);
    u32  pop();

    void dump_bytes(unsigned addr, unsigned count);

    void cmd_help();
    void cmd_obj();
    void cmd_fs();
    void cmd_read();
    void cmd_mem();

public:
    explicit ShellVM(ShellIn &io_): io{io_} {}

    void run(const ShellNode &node);
};

#endif
