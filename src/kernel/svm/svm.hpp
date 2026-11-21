#ifndef __kernel_basicshell_svm_hpp
#define __kernel_basicshell_svm_hpp

#include <global/type.hpp>
#include <interface/svm/op.hpp>

namespace svm {

/**
 * System Virtual Machine：一个值栈 + 指令循环。
 *
 * 它是内核基础设施：不知道命令名、不格式化、不分页。需要产出数据的指令
 * （Read/Mem/Obj/Fs）**多收一个操作数——输出缓冲区指针**，压栈顺序是
 * "缓冲区在前、参数在后"，所以弹的时候参数先出来、缓冲区最后。
 * 怎么显示由前端从缓冲区里读出来决定。
 */
class SysVM {
    // 值栈：定长，压/弹就是 sp 加减。现在栈上只有整数对象。
    static constexpr unsigned stack_max = 64;
    u32 stack[stack_max];
    unsigned sp = 0;

    bool exec(const Insn &in);
    void push(u32 v);
    u32  pop();

public:
    SysVM();

    /**
     * 执行一段指令。失败（栈下溢、容量不够、未知操作码）时输出缓冲区保持
     * 已写到的位置，前端按 len 判断拿到了多少。
     */
    void run(const ShellNode &node);
};

} // namespace svm

/** 内核里唯一的那个 VM（Kernel 构造时设好）。 */
extern svm::SysVM *sysvm;

#endif
