#pragma once

#include <global/memory.hpp>

namespace mem {

constexpr size_t SizeClassCount = 17;

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
    };/**
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

    SubZone* createzone(u32 level);
    void deletezone(SubZone* zone);
    void format_subzone(SubZone *, u32);

    // Object box manage
    void deleteobj(ObjHeader *);

    // search
    PageHead *search_page(void *);
    SubZone *search_zone(void *, PageHead *);

    KernelHeapFormat(void* page_base);
};

}