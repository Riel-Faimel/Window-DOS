#ifndef __kernel_svm_svm_hpp
#define __kernel_svm_svm_hpp

#include <global/type.hpp>
#include <interface/svm/op.hpp>
#include <interface/svm/data.hpp>

namespace svm {

/**
 * System Virtual Machine：一个带类型的值栈 + 四个寄存器 + 指令循环。
 *
 * 它是内核基础设施：不知道命令名、不格式化、不分页。需要产出数据的指令多收
 * 一个输出缓冲区操作数（Buf），怎么显示由前端决定。
 *
 * 栈上每个元素、每个寄存器都是容许数据 Value。没有独立的引用表：Ref 的 value
 * 就是栈槽下标，它指向的变量必须就在栈上。寄存器由指令自己点名（insn.reg），
 * 不是可以间接寻址的下标。
 */
class SysVM {
    // 值栈：定长，压/弹就是 sp 加减。
    static constexpr unsigned stack_max = 64;
    Value stack[stack_max];
    unsigned sp = 0;

    static constexpr unsigned reg_max = 4;
    Value reg[reg_max];

    bool exec(const Insn &in);
    void push(Value v);
    Value pop();

    Value *slot_of(const Value &ref);    // Ref -> 栈槽位置；不是 Ref 返回 nullptr
    Value  value_of(const Value &v);     // 取值：Ref 解引用，其余原样

public:
    SysVM();

    /**
     * 执行一段指令。失败（栈下溢、类型不符、容量不够、未知操作码）时报错并清栈，
     * 输出缓冲区保持已写到的位置。
     */
    void run(const ShellNode &node);
};

} // namespace svm

/** 内核里唯一的那个 VM（Kernel 构造时设好）。 */
extern svm::SysVM *sysvm;

#endif