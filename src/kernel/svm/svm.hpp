#ifndef __kernel_svm_svm_hpp
#define __kernel_svm_svm_hpp

#include <global/type.hpp>
#include <interface/svm/op.hpp>

namespace svm {

/**
 * 一次执行的状态：值栈 + 寄存器。
 *
 * 所有可变的东西都在这里，所以 SysVM 本身没有状态，一个 VM 可以服务多个执行流。
 * 栈上每个元素、每个寄存器都是容许数据 Value；没有独立的引用表：Ref 的 value
 * 就是栈槽下标，它指向的变量必须就在栈上；Reg 的 value 是寄存器下标。
 */
class SysVMThread {
public:
    static constexpr unsigned stack_max = 64;
    static constexpr unsigned reg_max = 4;

    SysVMThread();

    /** 清栈、清寄存器。*/
    void reset();

    void  push(Value v);
    Value pop();

    /** Ref -> 栈槽位置，Reg -> 寄存器位置；其它返回 nullptr。*/
    Value *slot_of(const Value &where);

    /** 取值：位置解一层引用，其它原样。*/
    Value  value_of(const Value &v);

private:
    Value stack[stack_max];
    unsigned sp = 0;
    Value reg[reg_max];

    friend class SysVM;
};

/**
 * 指令执行器。无状态：不持有栈、不持有寄存器，只认指令和容许数据。
 */
class SysVM {
public:
    SysVM();

    /**
     * 分配一份执行状态。调用之后由调用者持有：用完自己 delete。
     */
    SysVMThread *create_thread();

    /**
     * 在 th 上执行一段指令。失败（栈下溢、类型不符、容量不够、未知操作码）
     * 时返回 false，输出缓冲区保持已写到的位置。
     */
    bool run(SysVMThread &th, const ShellNode &node);

private:
    bool exec(SysVMThread &th, const Insn &in);
};

} // namespace svm

/** 内核里唯一的那个 VM（Kernel 构造时设好）。 */
extern svm::SysVM *sysvm;

#endif