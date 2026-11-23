#include "_inpage.hpp"

namespace arch {

namespace {

constexpr size_t page_size = 4096;
constexpr u32 space_inherit = 0; // 默认构造值：继承当前空间
constexpr u32 space_invalid = 0xFFFFFFFFu;

// PDE：P(0) | RW(1) | US(2) | PS(7)
constexpr u32 pde_kernel_page = 0x87;

} // namespace

/**
 * 建内核页目录并开启分页。分页的全部知识都在这里，不对外暴露。
 *
 * 页目录的内存由本实例持有（kernel_pd），构造函数里从物理页分配器拿，
 * 因为 CR3 要求 4KB 对齐。失败时静默返回、不设 CR0.PG：调用方无法回滚，
 * 但退化成普通堆仍然是正确的行为。
 */
void inpage::enable_paging() {
    u32 eax = 1, ebx = 0, ecx = 0, edx = 0;
    asm volatile ("cpuid" : "+a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx));
    if (!(edx & (1u << 3))) return; // PSE

    auto gen = physicalpage.aloc(mem::PageNum{1}, true);
    auto it = gen.begin();
    auto end = gen.end();
    if (!(it != end)) return;

    auto pkg = *it;
    kernel_pd = reinterpret_cast<u32 *>(
        static_cast<u32>(static_cast<mem::Pointer<u8>>(pkg.base))
    );
    if (!kernel_pd) return;

    for (u32 i = 0; i < 1024; i++)
        kernel_pd[i] = (i << 22) | pde_kernel_page;

    u32 cr4;
    asm volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1u << 4); // PSE
    asm volatile ("mov %0, %%cr4" : : "r"(cr4));

    const u32 pd = reinterpret_cast<u32>(kernel_pd);
    asm volatile ("mov %0, %%cr3" : : "r"(pd) : "memory");

    u32 cr0;
    asm volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80010000u; // PG | WP
    asm volatile ("mov %0, %%cr0" : : "r"(cr0) : "memory");
}

inpage::inpage(SuperBlock *s): Allocable{s} {
    enable_paging();
}

inpage::~inpage() {
    // 页目录是构造函数为这个实例拿的，析构时还回去。
    if (!kernel_pd) return;
    physicalpage.dloc({
        mem::Address{reinterpret_cast<u32>(kernel_pd)},
        mem::PageNum{1}
    });
    kernel_pd = nullptr;
}

/**
 * 句柄 → (cr3, ldtr)。
 *
 * 非虚函数：这层翻译是 x86 自己的知识，不属于 Allocable 的契约，
 * 只有 x86 在创建线程时调用一次。MemSpace 的取值语义也只在这里解释：
 *   0          继承当前空间
 *   0xFFFFFFFF 无效
 *   其它       页目录物理地址
 */
void inpage::to_hardware(mem::MemSpace space, size_t &cr3, u16 &ldtr) const {
    const u32 id = static_cast<u32>(space);

    if (id == space_inherit) {
        // 句柄里没有硬件状态，继承只能向 CPU 要。
        asm volatile ("mov %%cr3, %0" : "=r"(cr3));
        asm volatile ("sldt %0" : "=m"(ldtr));
        return;
    }

    cr3 = (id == space_invalid) ? 0 : id;
    ldtr = 0;
}

/* === 空间相关 === */

mem::MemSpace inpage::alloc() {
    if (!kernel_pd) return mem::MemSpace{space_invalid};   // 没有内核页目录，谈不上新空间

    auto gen = physicalpage.aloc(mem::PageNum{1}, true);
    auto it = gen.begin();
    auto end = gen.end();
    if (!(it != end)) return mem::MemSpace{space_invalid};

    auto pkg = *it;
    const u32 pd = static_cast<u32>(static_cast<mem::Pointer<u8>>(pkg.base));

    __builtin_memset(reinterpret_cast<void *>(pd), 0, page_size);

    // 新空间里内核代码与内核堆必须同样可达：复制内核页目录。
    auto entries = reinterpret_cast<u32 *>(pd);
    for (u32 i = 0; i < 1024; i++) entries[i] = kernel_pd[i];

    return mem::MemSpace{pd};
}

void inpage::dlloc(mem::MemSpace space) {
    const u32 pd = static_cast<u32>(space);
    if (pd == space_inherit || pd == space_invalid) return;
    physicalpage.dloc({mem::Address{pd}, mem::PageNum{1}});
}

void *inpage::alloc(mem::MemSpace space, size_t size, mem::Allocable::MemAttr attr) {
    // 继承（0）是合法的：就在当前空间里拿页；只有无效句柄拒绝。
    if (static_cast<u32>(space) == space_invalid || !size) return nullptr;

    const size_t pages = (size + page_size - 1) / page_size;

    auto gen = physicalpage.aloc(mem::PageNum{static_cast<u32>(pages)}, true);
    auto it = gen.begin();
    auto end = gen.end();
    if (!(it != end)) return nullptr;

    auto pkg = *it;
    const u32 base = static_cast<u32>(static_cast<mem::Pointer<u8>>(pkg.base));

    // 指定了地址却没拿到：归还并报"这里放不下"，由调用方决定是否换址重试。
    if (attr.want && base != attr.want) {
        physicalpage.dloc({mem::Address{base}, mem::PageNum{static_cast<u32>(pages)}});
        return nullptr;
    }

    // 恒等映射：物理地址即虚拟地址。
    return reinterpret_cast<void *>(base);
}

void inpage::dlloc(mem::MemSpace space, void *ptr, size_t size) {
    if (static_cast<u32>(space) == space_invalid || !ptr) return;

    const size_t pages = (size + page_size - 1) / page_size;
    if (!pages) return;

    physicalpage.dloc({
        mem::Address{reinterpret_cast<u32>(ptr)},
        mem::PageNum{static_cast<u32>(pages)}
    });
}

/* === 空间无关：内核堆在所有空间里都可达 === */

void *inpage::alloc(size_t size) {
    const int level = size_to_level(size);

    if (level == -1) {
        // 超过最大 size class：按整页分配。
        const auto need = static_cast<mem::PageNum>(
            mem::ByteNum{static_cast<u32>(size + sizeof(ObjHeader))}
        );

        auto gen = physicalpage.aloc(need, true);
        auto it = gen.begin();
        auto end = gen.end();
        if (!(it != end)) return nullptr;

        auto pkg = *it;
        auto box = mem::Pointer<ObjHeader>{pkg.base};
        if (box == (void *)-1) return nullptr;

        box->pages = pkg.pages;
        box->head_guard = mem::ObjectSingalGuard;
        return box + 1;
    }

    auto &objzone = sb->free_zones[level];
    if (objzone.obj_start == nullptr) {
        if (!createzone(level)) return nullptr;
    }

    auto box = objzone.obj_start;
    if (!box) return nullptr;

    objzone.free_count--;
    objzone.obj_start = box->next;

    // level 要在构造之后再写：header 的 union 起始值是 next = nullptr。
    new (box) ObjHeader{.next = nullptr, .head_guard = mem::ObjectHeadGuard};
    box->level = level;
    *reinterpret_cast<u16 *>(
        reinterpret_cast<u8 *>(box + 1) + mem::size_classes[level]
    ) = mem::ObjectTailGuard;

    return box + 1;
}

void inpage::dlloc(void *ptr, size_t) {
    if (!ptr) return;

    auto *objhead = static_cast<ObjHeader *>(ptr) - 1;

    if (objhead->head_guard == mem::ObjectSingalGuard) {
        objhead->head_guard = 0;
        physicalpage.dloc({
            mem::Address{objhead},
            static_cast<mem::PageNum>(objhead->pages)
        });
    } else if (objhead->head_guard == mem::ObjectHeadGuard) {
        objhead->head_guard = 0;
        auto &zone = sb->free_zones[objhead->level];
        objhead->next = zone.obj_start;
        zone.obj_start = objhead;
        zone.free_count++;
    }
}

} // namespace arch
