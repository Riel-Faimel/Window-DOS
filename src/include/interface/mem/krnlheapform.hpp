#ifndef __include_interface_mem_krnlheapform_hpp
#define __include_interface_mem_krnlheapform_hpp

#include <global/memory.hpp>

namespace mem {

constexpr size_t SizeClassCount = 17;

// 堆格式的常量。谁实现/使用这个格式都要用，所以放在格式自己的头里，
// 不留在 mm 的私有头。
#pragma GCC diagnostic push
constexpr size_t MemClusterSize = 4096;
constexpr u32 SuperBlockMagic = 'R'|'S'<<8|'B'<<16|'M'<<24;
constexpr u32 PageHeadMagic = 'R'|'P'<<8|'H'<<16|'M'<<24;
constexpr u16 ObjectHeadGuard = 'H'|'G'<<8;
constexpr u16 ObjectTailGuard = 'T'|'G'<<8;
constexpr u16 ObjectSingalGuard = 'S'|'P'<<8;
#pragma GCC diagnostic pop

constexpr u16 size_classes[SizeClassCount] = {
    16, 24, 32, 48, 64, 96, 128, 192, 256,
    384, 512, 768, 1024, 1536, 2048, 3072, 4096
};

class KernelHeapFormat {
protected:
#pragma pack(push, 1)
    struct ObjHeader {
        union {
            Pointer<ObjHeader> next; // in list
            BoxLevel level; // box allocated
            SegmentSize pages; // signal allocated
        };
        u16 head_guard;
        // Keep sizeof(ObjHeader) == 8 so the payload after the header is
        // naturally aligned and every size-class slot can stay 8-aligned.
        u16 _pad = 0;
    };
    static_assert(sizeof(ObjHeader) == 8, "ObjHeader must keep payload 8-aligned");
    /**
     * with u16 tail_guard behand object
     */

    struct SubZone {
        void *end = nullptr;
        u16 objsize = 0;
        enum class Type : u16 {
            nul,
            end,
            inl,
        } type = Type::end;

        bool is_end_dc();
        bool is_free_dc();
    };

    struct ObjZone {
        ObjHeader *obj_start = nullptr;
        BoxNum obj_count = 0;
        BoxNum free_count = 0;
        u32 level = 0;
    };

    struct PageHead {
        u32 magic;
        PageHead* prev = nullptr;
        PageHead* next = nullptr;
        void *page_end = 0;

        PageHead(void *endaddr);
        PageHead *next_node() { return next; }
        u32 get_free_size();

        struct tran {
            SubZone *base;
            tran(PageHead *p): base{(SubZone *)(p+1)}{}
            tran(SubZone *sz): base{sz}{}

            struct iterator {
                SubZone *ptr;
                u8 done = 2;

                iterator(SubZone *p): ptr{p}{}
                SubZone &operator*() { return *ptr; }
                bool operator!= (const iterator &) { 
                    if (done == 1) {
                        done--;
                        return true;
                    };
                    return done; 
                }
                iterator &operator++();
            };
            iterator begin() { return {base}; }
            iterator end() { return {nullptr}; }
        };
        tran subzone_tranveser() { return {this}; }
        tran subzone_tranveser(SubZone *ptr) { return {ptr}; }

        bool create_subzone(SubZone *free_sz, size_t size);
    };

    struct SuperBlock {
        u32 magic;
        PageHead* page_head = nullptr;
        ObjZone free_zones[SizeClassCount] = {};
        PageNum total_pages = PageNum{0u};
    };
#pragma pack(pop)

    SuperBlock* sb = nullptr;

    /**
     * format: input one L_Page, format as 
     * -------------------------->
     * |super block|page head|free
     * 
     * include: input one L_Page, format and
     * regist into super block(need formated)
     */
    void format(PagePackage);
    PageHead *include(PagePackage);

    static int size_to_level(u32 size);

    // Every slot is rounded up to this alignment: the payload starts at
    // box + sizeof(ObjHeader) and all slot starts are aligned too.
    static constexpr size_t HeapAlign = 8;
    static constexpr size_t slot_size(u32 level) {
        const size_t raw =
            sizeof(ObjHeader) + size_classes[level] + sizeof(u16);
        return (raw + HeapAlign - 1) & ~(HeapAlign - 1);
    }

    SubZone* createzone(u32 level);
    void deletezone(SubZone* zone);
    void format_subzone(SubZone *, u32);

    // Object box manage
    void deleteobj(ObjHeader *);

    // search
    PageHead *search_page(void *);
    SubZone *search_zone(void *, PageHead *);

    KernelHeapFormat(SuperBlock* page_base);
public:
    using SuperBlock = SuperBlock;

    /**
     * Heap accounting for MemoryManager::status().
     * total: object-area capacity in the page list (metadata excluded)
     * alloced: slot bytes handed to live objects
     * used: payload bytes of live objects (size class, < alloced)
     * free: free slot bytes + unformatted subzone bytes
     * Invariant: total == alloced + free, used <= alloced.
     */
    static void heap_status(
        const SuperBlock *sb,
        size_t &total, size_t &alloced, size_t &used, size_t &free
    );
};

}

#endif