#include "bs.hpp"
#include <interface/svm/op.hpp>
#include <interface/svm/data.hpp>
#include <svm/svm.hpp>
#include <stdio>
#include <SrvDevMgr/SDM.hpp>

using namespace svm;

namespace {

// 编译器自己用的槽：Open 解析出来的对象放在这一格，随后用 Ref 引用它。
constexpr u32 disk_slot = 60;

// 脚本变量从这一格往上长，最多 vars_max 个（到 59 为止，60 留给 disk_slot）。
constexpr u32 var_base = 48;
constexpr unsigned vars_max = 12;

// 按空格拆参，丢掉空词（连续空格会拆出空串）。
rtl::array<String> split_args(const String &line) {
    rtl::array<String> argv;
    for (auto &w : line.split(' '))
        if (!w.empty()) argv.append(rtl::move(w));
    return argv;
}

// 严格十六进制解析：整个词都必须是数字，不留“读到非法字符就停”的洞。
bool parse_hex(const String &w, u32 &re) {
    if (w.empty()) return false;
    u32 v = 0;
    for (char c : w) {
        unsigned d;
        if (c >= '0' && c <= '9') d = unsigned(c - '0');
        else if (c >= 'a' && c <= 'f') d = unsigned(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') d = unsigned(c - 'A' + 10);
        else return false;
        v = (v << 4) | d;
    }
    re = v;
    return true;
}

/**
 * 前端的命令表：操作码 → 名字、帮助、参数签名、是否需要输出。
 *
 * 参数顺序 = push 的顺序 = 执行器 pop 的反序；缺省值和范围只在这里写一份，
 * help 的输出也从这张表生成。表是前端的东西，SysVM 看不到它。
 */
const BasicShell::OpInfo table[] = {
    { Op::Read, "read", "dump sectors of the n-th physical disk", 3, {
        { "disk",  0, 0, 15, BasicShell::ArgKind::Disk },
        { "lba",   0, 0, 0  },
        { "count", 1, 1, 8  },
    } },
    { Op::Mem,  "mem",  "dump memory (identity mapped)", 2, {
        { "addr",  0,  0, 0    },
        { "count", 64, 1, 4096 },
    } },
    { Op::Status, "Status", "all | mm | page | obj | fs", 0, {} },
};

constexpr unsigned table_n = sizeof(table) / sizeof(table[0]);

} // namespace

inline const BasicShell::OpInfo *op_lookup(const String &name) {
    for (unsigned i = 0; i < table_n; i++)
        if (name == table[i].name) return &table[i];

    // The command is documented as Status; accept the lowercase spelling too.
    if (name == String("status")) {
        for (unsigned i = 0; i < table_n; i++)
            if (table[i].op == Op::Status) return &table[i];
    }
    return nullptr;
}

inline const BasicShell::OpInfo *op_table(unsigned &count) {
    count = table_n;
    return table;
}