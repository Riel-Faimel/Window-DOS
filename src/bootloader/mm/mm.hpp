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
        u32 obj_count = 0;
        u32 free_count = 0;
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

        void create_subzone(SubZone *free_sz, size_t size);
    };

    struct SuperBlock {
        u32 magic;
        PageHead* page_head = nullptr;
        ObjZone free_zones[__window_dos_define_0x1_SIZE_CLASS_COUNT] = {};
        u32 total_pages = 0;
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
    void format(address_package);
    PageHead *include(address_package);

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

struct MemoryManager {
    class allocable {
    public:
        inline allocable(MemoryManager *);
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
        nopage(void* page_base, MemoryManager *);
        
        void *alloc(size_t);
        void dlloc(void *, size_t);
    };
    struct inpage : public allocable, KernelHeapFormat{
        inpage(void* page_base, MemoryManager *);

        void *alloc(size_t);
        void dlloc(void *, size_t);
    };

    union {
        nopage _rn;
        inpage _ri;
    } reserved_space;
    allocable *_;


public:
    MemoryManager(bool print = true);

    void *alloc(size_t size) {
        return _->alloc(size);
    }
    void dlloc(void *ptr, size_t size = -1) {
        _->dlloc(ptr, size);
    }
};

#endif