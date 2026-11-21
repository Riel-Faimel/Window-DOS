#pragma once
#include <global/type.hpp>
#include <lib/string>
#include <HL/container>

namespace svm {

/**
 * 操作码。SysVM 只认这些；它不知道它们背后是哪条命令、怎么显示。
 */
enum class Op : u16 {
    None = 0,

    Help,       // 前端处理，不提交给 VM（它只是个占位，让命令表有名字可查）
    Push,       // 把立即数压栈（现在只有整数对象）
    Read,       // 读扇区，原始字节写进输出缓冲区
    Mem,        // 读内存，原始字节写进输出缓冲区
    Obj,        // 枚举内核对象，写 ObjRec 记录
    Fs,         // 枚举盘符，写 ObjRec 记录
};

/**
 * 一条指令。
 */
struct Insn {
    Op  op  = Op::None;
    u32 imm = 0;
};

/**
 * 原始输出缓冲区。SysVM 只往里写字节：它不格式化、不换行、不分页，
 * 怎么显示由提交指令的那一方决定。
 */
struct OutBuf {
    u8 *data = nullptr;
    unsigned cap = 0;
    unsigned len = 0;

    bool put(const void *src, unsigned n) {
        if (!data || n > cap - len) return false;
        auto *p = static_cast<const u8 *>(src);
        for (unsigned i = 0; i < n; i++) data[len + i] = p[i];
        len += n;
        return true;
    }

    template<typename T>
    bool put_as(const T &v) { return put(&v, sizeof(T)); }

    void reset() { len = 0; }
};

/** Obj / Fs 交出来的记录类型。 */
enum class RecKind : u8 {
    Phys = 0,   // a = 物理盘地址
    Part,       // a = 起始 LBA，b = 扇区数，c = 分区类型
    Serial,     // 串口
    Frame,      // 帧口
    Unknown,
    Drive,      // a = 盘符字符，b = cid
};

struct ObjRec {
    RecKind kind;
    u32 a;
    u32 b;
    u32 c;
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

/**
 * 参数描述：名字、缺省值、取值范围。这是前端的东西——SysVM 只收到编译好的
 * 指令，看不到名字、缺省值和范围。
 *
 * 命令行的两种写法都认：
 *   read 0 1E8 2             位置参数，按表里顺序填
 *   read disk|0 lba|1E8      命名参数，name|value，顺序随意
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
    bool        out;     // 需要输出：编译时会多压一个缓冲区指针操作数
    OpArg       args[3];
};

} // namespace svm
