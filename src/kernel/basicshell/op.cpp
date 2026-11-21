#include "op.hpp"

namespace {

// 参数顺序 = push 的顺序 = 执行器 pop 的反序。
const OpInfo table[] = {
    { Op::Help, "help", "this list", 0, {} },
    { Op::Obj,  "obj",  "kernel objects (physical/logical disks, ports)", 0, {} },
    { Op::Fs,   "fs",   "drive letters registered in DLS", 0, {} },
    { Op::Read, "read", "dump sectors of the n-th physical disk", 3, {
        { "disk",  0, 0, 15 },    // 0..15
        { "lba",   0, 0, 0  },    // 不限
        { "count", 1, 1, 8  },    // 缺省 1，最多 8
    } },
    { Op::Mem,  "mem",  "dump memory (identity mapped)", 2, {
        { "addr",  0,  0, 0    },
        { "count", 64, 1, 4096 }, // 缺省 64 字节
    } },
};

constexpr unsigned table_n = sizeof(table) / sizeof(table[0]);

} // namespace

const OpInfo *op_lookup(const String &name) {
    for (unsigned i = 0; i < table_n; i++)
        if (name == table[i].name) return &table[i];
    return nullptr;
}

const OpInfo *op_table(unsigned &count) {
    count = table_n;
    return table;
}
