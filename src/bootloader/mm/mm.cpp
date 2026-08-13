#include "_mm.hpp"

MemoryManager::allocable *allocator;

KernelHeapFormat::KernelHeapFormat(void *page_base): sb{nullptr}{
    if (page_base == nullptr) 
    for (auto pkg : physicalpage->aloc(4096, true)) {
        //only one time
        format(pkg);
    }
    if (page_base && static_cast<SuperBlock *>(page_base)->magic == SuperBlockMagic) {
        sb = static_cast<SuperBlock *>(page_base);
    }
}
void KernelHeapFormat::format(address_package addrpkg) {
    #pragma pack(push, 1)
    struct head {
        SuperBlock sb;
        PageHead ph;
    };
    #pragma pack(pop)

    sb = (SuperBlock *)new (reinterpret_cast<void *>(addrpkg.address.low)) head {
        .sb = { .magic = SuperBlockMagic, .total_pages = addrpkg.len.low },
        .ph = { .magic = PageHeadMagic, .page_end = (void *)((addrpkg.address+addrpkg.len).low) }
    };
    sb->page_head = (PageHead *)(sb+1);
}
void KernelHeapFormat::include(address_package addrpkg) {
    PageHead *page = new (reinterpret_cast<void *>(addrpkg.address.low)) PageHead {
        .magic = PageHeadMagic, .next = sb->page_head,
        .page_end = (void *)((addrpkg.address+addrpkg.len).low)
    };
    sb->page_head = page;
}
/*
KernelHeapFormat::ObjHeader *KernelHeapFormat::findspace(KernelHeapFormat::SubZone *sz) {
    if (sz->free_count) return nullptr;
    for (unsigned i = 0;i < sz->block_count;i++) {
        void *objptr = sz->subzone_start + (i * size_classes[sz->level]);
        if (static_cast<ObjHeader *>(objptr)->head_guard != ObjectHeadGuard) {
            sz->free_count--;
            return static_cast<ObjHeader *>(objptr)+1;
        }
    }
}
*/
int KernelHeapFormat::size_to_level(u32 size) {
    u32 label = 0;
    for (auto l : size_classes) {
        if (l > size) return label;
        label++;
    }
    return (u32)-1;
}
KernelHeapFormat::PageHead *KernelHeapFormat::search_page(KernelHeapFormat::ObjHeader *obj) {
    for (auto page : rtl::list_tranveser{sb->page_head}) {
        ;
    }
}
KernelHeapFormat::SubZone *KernelHeapFormat::search_zone(KernelHeapFormat::ObjHeader *obj, KernelHeapFormat::PageHead *page) {
    //for (auto zone : rtl::list_tranveser(reinterpret_cast<SubZone *>(page+1)));
}
KernelHeapFormat::SubZone *KernelHeapFormat::createzone(
    KernelHeapFormat::PageHead *page, u32 level, u32 block_count
) {
    ;
}
void KernelHeapFormat::deletezone(KernelHeapFormat::SubZone *zone) {
    ;
}


inline MemoryManager::allocable::allocable() { allocator = this; }

MemoryManager::nopage::nopage(void* page_base):
KernelHeapFormat{page_base} {}
void *MemoryManager::nopage::alloc(size_t size) {
    KernelHeapFormat::ObjHeader *box = nullptr;
    auto level = size_to_level(size);
    if (level == -1) {
        // alloc as page
        unsigned need_page = (size + sizeof(ObjHeader) - 1)/4096 + 1;
        for(auto addrpkg : physicalpage->aloc(need_page, true))
        box = (ObjHeader *)addrpkg.address.low;

        if(box == (void *)-1) return nullptr;
        box->next = (ObjHeader *)need_page;
        box->head_guard = ObjectSingalGuard;
        return box+1;
    }

    auto &objzone = sb->free_zones[level];
    //cout << "litte aloc" << sb << ", " << objzone.obj_start << '\n';
    if (objzone.obj_start == nullptr) {
        // need to new subzone
        createzone(nullptr, level, 0);
    }

    box = objzone.obj_start;
    objzone.free_count--;
    objzone.obj_start = box->next;
    box->next = (ObjHeader *)level;

    // set head and guard
    new (box) ObjHeader{.next = nullptr, .head_guard = ObjectHeadGuard};
    *reinterpret_cast<u16 *>(
        reinterpret_cast<u8 *>(box + 1) + size_classes[level]
    ) = ObjectTailGuard;

    return box+1;
}
void MemoryManager::nopage::dlloc(void *ptr, size_t) {
    auto objhead = static_cast<ObjHeader *>(ptr)-1;
    if (objhead->head_guard == ObjectSingalGuard) {
        objhead->head_guard = 0;
        physicalpage->dloc({(u32)objhead, (u32)objhead->next});
    } else if (objhead->head_guard == ObjectHeadGuard) {
        objhead->head_guard = 0;
        auto &zone = sb->free_zones[(u32)objhead->next];
        objhead->next = zone.obj_start;
        zone.obj_start = objhead;
        zone.free_count++;
    }
}

MemoryManager::inpage::inpage(void* page_base):
KernelHeapFormat{page_base} {
    while(1);
}
void *MemoryManager::inpage::alloc(size_t) {}
void MemoryManager::inpage::dlloc(void *, size_t) {}

MemoryManager::MemoryManager():
phypage{}, reserved_space{._rn{nullptr}} {}

//======

void *operator new(size_t size) {
    return allocator->alloc(size);
}
void operator delete(void *ptr) {
    return allocator->dlloc(ptr, -1);
}
void operator delete(void *ptr, size_t size) {
    return allocator->dlloc(ptr, size);
}

void *operator new[](size_t size) {
    return allocator->alloc(size);
}
void operator delete[](void *ptr) {
    return allocator->dlloc(ptr, -1);
}
void operator delete[](void *ptr, size_t size) {
    return allocator->dlloc(ptr, size);
}

/**
 *     ^
 *    | |
 *    | |
 *  ( ) ( )
 */
