#ifndef __bootloader_mm_mm_hpp
#define __bootloader_mm_mm_hpp

#include <global/type.hpp>
#include <global/new.hpp>

#pragma pack(push, 1)
struct address_package {
    u64 address;
    u64 len;
};

class PhysicalPage {
public:
struct mem_list {
    address_package mem;
    mem_list *next;
    u8 exist;
    u8 r1;
    u8 r2;
    u8 r3;
};
#pragma pack(pop)

class address_generator {
    size_t pageneedednums;
    bool need_continuous;
    PhysicalPage* memmgrptr;

    friend class iterator;
public:
    class iterator {
        bool no_mem;
        bool done;
        size_t count;
        address_generator* conditions;
    public:
        iterator(address_generator*);

        address_package operator* ();
        bool operator!= (iterator&);
        iterator& operator++ ();
    };

    address_generator(size_t, bool, PhysicalPage*);
    ~address_generator();

    iterator begin();
    iterator end() const;
};

    PhysicalPage();
    ~PhysicalPage();

    address_generator aloc(size_t page_nums, bool need_continuous = false);
    unsigned dloc(address_package mem);
    unsigned neaten();

private:
    friend class Iterator;
    static mem_list* mem_list_root;

    class Iterator {
        unsigned count = 0;
        bool done = false;
    public:
        mem_list *operator* ();
        bool operator!= (Iterator&);
        Iterator& operator++ ();
    };
    mem_list *findspace();
    Iterator begin();
    Iterator end();
};


#define SIZE_CLASS_COUNT 17

class KernelHeapFormat {
protected:
    struct PageHead;
    struct SubZone;
#pragma pack(push, 1)
    struct SuperBlock {
        u32 magic;
        PageHead* page_head;
        SubZone* free_zones[SIZE_CLASS_COUNT];
        u32 total_pages;
    };

    struct SubZone {
        SubZone* prev;
        SubZone* next;
        
        PageHead* owner;
        u32 block_count;
        u32 free_count;
        u32 level;
        u8 *subzone_start;
    };

    struct PageHead {
        u32 magic;
        PageHead* prev;
        PageHead* next;
        u32 page_count;
        u16 zone_count;
        SubZone zones[];
    };

    struct ObjHeader {
        SubZone* zone;
        u16 head_guard;
    };/**
     * with u16 tail_guard behand object
     */
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

    // 辅助
    static u32 size_to_level(u32 size);
    u32 obj_total_size(u32 level);

    // 子区管理
    SubZone* createzone(PageHead* page, u32 level, u32 block_count);
    void deletezone(SubZone* zone);

    // Object box manage
    ObjHeader *findspace(SubZone *);
    void deleteobj(ObjHeader *);

    // 校验
    bool obj_verify(ObjHeader* obj);

    KernelHeapFormat(void* page_base);
};

class MemoryManager {
public:
class allocable {
public:
    allocable();
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
class nopage : public allocable, KernelHeapFormat{
public:
    nopage(void* page_base);
    
    void *alloc(size_t);
    void dlloc(void *, size_t);
};
class inpage : public allocable, KernelHeapFormat{
public:
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