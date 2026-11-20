#pragma once

#include <global/type.hpp>
#include <interface/mem/allocable.hpp>

namespace arch {
    
class inpage : public mem::Allocable {
    u32 *kernel_pd = nullptr;   // 本实例持有的内核页目录
    // 建内核页目录并开启分页；只在构造函数里调用。
    void enable_paging();
public:
    explicit inpage(SuperBlock *);
    ~inpage() override;

    /**
     * 句柄 → 硬件状态。非虚：这是 x86 的知识，不进 Allocable 的虚表，
     * 只有 x86 自己在创建线程时调用一次。
     */
    void to_hardware(mem::MemSpace, size_t &cr3, u16 &ldtr) const;

    mem::MemSpace alloc() override;
    void dlloc(mem::MemSpace) override;
    void *alloc(mem::MemSpace, size_t, mem::MemAttr) override;
    void dlloc(mem::MemSpace, void *, size_t) override;
    void *alloc(size_t) override;
    void dlloc(void *, size_t = -1) override;
};

} // namespace arch 