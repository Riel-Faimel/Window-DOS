#pragma once
#include <global/type.hpp>
#include <lib/string>
#include <HL/container>

namespace svm {

// SysVM 容许数据
struct Value {
    enum class TypeID : u32 {
        Nil = 0,
        Reg,        // 寄存器索引
        Int,        // 整数值
        Ref,        // 另一个变量在栈上的槽下标

        Ordering,   // 比较结果 -1 / 0 / 1
        ReadOut,    // ReadResult*
        MemOut,     // MemResult*
        ObjOut,     // ObjResult*  —— 与 FsOut 必须是不同的枚举值
        FsOut,      // FsResult*
        StatusOut,  // StatusResult*
        Obj,        // 指向内核对象的指针
    };

    TypeID type = TypeID::Nil;
    u32    value = 0;
};

/**
 * 操作码。SysVM 只认这些；它不知道它们背后是哪条命令、怎么显示。
 */
enum class Op : u32 {
    None = 0,

    Push,       // 压容许数据

    // 修改栈内容：赋值语义，类型必须相同或协变
    Assign,     // imm 必须是 Ref：把栈顶赋值给栈上第 imm.value 个数据
    Drop,
    Swap,       // 交换两个同类型的值

    // 比较：只接受强序类型（Int / Ref / Obj），把 栈顶 <=> imm 的 Ordering 压回去；
    // Ordering 自己是无序的，参与比较就是 fault
    Cmp,

    // 控制流
    Jmp,        // 无条件跳转，imm 是目标下标
    Jc,         // 条件跳转：imm 是 Ordering 条件值（-1/0/1），和栈顶 Ordering 比，相等就跳到「栈顶下面一层」的 Int；不弹任何东西

    Open,       // 弹 Int（盘号），解析成引用（Disk）后压回；结果会缓存
    Read,       // 弹 count、lba、ref(Disk) → ReadResult
    Mem,        // 弹 count、addr(Int) → MemResult
    Obj,        // 无操作数 → ObjResult
    Fs,         // 无操作数 → FsResult
    Status,     // 无操作数 → StatusResult
};

/**
 * 一条指令：操作码 + 两个操作数。
 *
 * 操作数是容许数据：字面量（Int / Obj）、槽引用（Ref）、或寄存器（Reg）。
 */
struct Insn {
    Op    op  = Op::None;
    Value imm = {Value::TypeID::Nil, 0};
    Value imm2= {Value::TypeID::Nil, 0}; // 第二个操作数
};

/**
 * 一段编译好的程序：指令序列 + 编译期错误。这是 VM 的输入，不是前端私有
 * 的东西——前端负责把它造出来。
 */
struct ShellNode {
    rtl::array<Insn> code;
    bool   ok = true;
    String error;
};

} // namespace svm