#pragma once

#include <global/type.hpp>
#include <interface/knlobj/objinfo.hpp>
#include <interface/srvdev/driveentry.hpp>

namespace svm {

// 每个输出指令一个结果类型；结果本身只保存最终数组的 addr/len，
// 不内嵌数组，也不拥有容器。数组由后端一次分配、直接写入。
struct ReadResult {
    u8 *addr = nullptr;
    size_t len = 0;
};

struct MemResult {
    u8 *addr = nullptr;
    size_t len = 0;
};

struct ObjResult {
    ObjEntry *addr = nullptr;
    size_t len = 0;
};

struct FsResult {
    DriveEntry *addr = nullptr;
    size_t len = 0;
};

struct StatusResult {
    // Module selector is a frontend/command value; SVM only carries it.
    u32 selector = 0;

    size_t total_mem = 0;
    size_t alloced_mem = 0;
    size_t used_mem = 0;
    size_t free_mem = 0;
    size_t page_total = 0;
    size_t page_free = 0;

    ObjEntry *objs = nullptr;
    size_t obj_len = 0;

    DriveEntry *drives = nullptr;
    size_t fs_len = 0;
};

} // namespace svm
