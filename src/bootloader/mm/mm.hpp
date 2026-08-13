#ifndef __bootloader_mm_mm_hpp
#define __bootloader_mm_mm_hpp

#include "phy.hpp"
#include <global/type.hpp>
#include <global/new.hpp>

#define __window_dos_define_0x1_SIZE_CLASS_COUNT 17

class KernelHeapFormat {
protected:
#pragma pack(push, 1)
    struct ObjHeader {
        ObjHeader* next;
        u16 head_guard;
    };/**
     * with u16 tail_guard behand object
     */

    struct SubZone {
        void *start = nullptr;
        void *end = nullptr;
        u32 obj_count = 0;
        u32 free_count = 0;
    };

    struct ObjZone {
        ObjHeader *obj_start = nullptr;
        u32 obj_count = 0;
        u32 free_count = 0;
        u32 level = 0;
    };

    struct PageHead {
        u32 magic;
        PageHead* prev = nullptr;
        PageHead* next = nullptr;
        void *page_end = 0;
        u16 zone_count = 0;
        //SubZone zones[];

        inline PageHead *next_node() { return next; }
    };

    struct SuperBlock {
        u32 magic;
        PageHead* page_head = nullptr;
        ObjZone free_zones[__window_dos_define_0x1_SIZE_CLASS_COUNT] = {};
        u32 total_pages = 0;
    };
#pragma pack(pop)

    SuperBlock* sb;

    /**
     * format: input one L_Page, format as 
     * -------------------------->
     * |super block|page head|free
     * 
     * include: input one L_Page, format and
     * regist into super block(need formated)
     */
    void format(address_package);
    void include(address_package);

    static int size_to_level(u32 size);

    // 子区管理
    SubZone* createzone(PageHead* page, u32 level, u32 block_count);
    void deletezone(SubZone* zone);

    // Object box manage
    void deleteobj(ObjHeader *);

    // search
    PageHead *search_page(ObjHeader *);
    SubZone *search_zone(ObjHeader *, PageHead *);

    KernelHeapFormat(void* page_base);
};

struct MemoryManager {
    class allocable {
    public:
        inline allocable();
        virtual void *alloc(size_t) = 0;
        virtual void dlloc(void *, size_t) = 0;
    };

private:
        /**
         * kernel heap allocator
         * when no paging, we use physical memory know from in paging
         * 
         * all of them offered alloc/dlloc function
         * and called by operator new/delete
         */
    struct nopage : public allocable, KernelHeapFormat{
        nopage(void* page_base);
        
        void *alloc(size_t);
        void dlloc(void *, size_t);
    };
    struct inpage : public allocable, KernelHeapFormat{
        inpage(void* page_base);

        void *alloc(size_t);
        void dlloc(void *, size_t);
    };

    PhysicalPage phypage;

    union {
        nopage _rn;
        inpage _ri;
    } reserved_space;

public:
    MemoryManager();

};

#endif