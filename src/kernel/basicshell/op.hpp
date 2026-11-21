#ifndef __kernel_basicshell_op_hpp
#define __kernel_basicshell_op_hpp

#include <global/type.hpp>
#include <lib/string>

/**
 * 字节码操作码。执行器 switch 它（编译成跳转表），不在运行时比较字符串；
 * 名字 → 枚举的查表只在前端做一次。
 */
enum class Op : u16 {
    None = 0,

    Push,       // 内部指令：把整数对象压栈，imm 是立即数
    Help,
    Obj,
    Fs,
    Read,
    Mem,
};

/**
 * 一条指令。现在只有 Push 用 imm；以后参数变成对象引用时，这里换成 Atom。
 */
struct Insn {
    Op  op  = Op::None;
    u32 imm = 0;
};

/**
 * 参数描述：名字、缺省值、取值范围。
 *
 * 命令行的两种写法都认：
 *   read 0 1E8 2             位置参数，按表里顺序填
 *   read disk|0 lba|1E8      命名参数，name|value，顺序随意
 *
 * 前端按它把文字参数编译成 push 指令；缺省值只在这里写一份。
 */
struct OpArg {
    const char *name;   // 命名参数用，也用于报错
    u32 def;
    u32 lo;
    u32 hi;             // hi == 0 表示不检查上限
};

struct OpInfo {
    Op          op;
    const char *name;
    const char *usage;
    u8          argc;
    OpArg       args[3];
};

/**
 * 名字 → 命令描述。未知名字返回 nullptr。
 * 这里用一次字符串比较没关系：发生在解析期，不在执行期。
 */
const OpInfo *op_lookup(const String &name);

/**
 * 操作码表（不含 Push/None），给 help 用；count 返回表长。
 */
const OpInfo *op_table(unsigned &count);

#endif
