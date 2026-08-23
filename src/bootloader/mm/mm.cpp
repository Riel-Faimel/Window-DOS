#include "_mm.hpp"

MemoryManager::allocable *allocator;


KernelHeapFormat::KernelHeapFormat(void *page_base){
    if (page_base == nullptr) {
        auto pkg = *physicalpage.aloc(4096, true).begin();
        format(pkg);
    } else if (static_cast<SuperBlock *>(page_base)->magic == SuperBlockMagic) {
        sb = static_cast<SuperBlock *>(page_base);
    }
}
KernelHeapFormat::SubZone *KernelHeapFormat::createzone(u32 level) {
    u32 size_need = size_classes[level]+sizeof(ObjHeader)+sizeof(u16);
    //cout << "size need = " << size_need << '\n';

    for (auto &page : rtl::list_tranveser{sb->page_head})
    if (page.get_free_size() > size_need) // page large enough
    for (auto &subzone : page.subzone_tranveser())
    if (subzone.is_free_dc()) {// free subzone
        for (auto &nz : page.subzone_tranveser((SubZone *)(subzone.end))) {
            if(!nz.is_free_dc()) break;
            // merge
            subzone.end = nz.end;
            nz.end = 0;
            nz.objsize = 0;
            nz.type = SubZone::Type::nul;
        }
        if ((u32)subzone.end - (u32)(&subzone+1) > size_need) { // large enough
            page.create_subzone(&subzone, size_need);
            format_subzone(&subzone, level);
            return &subzone;
        }
    }
    // not found
    for (auto addrpkg : physicalpage.aloc(512, true)) {
        if (addrpkg.address == u64{0}) addrpkg = *physicalpage.aloc(2, true).begin();
        auto page = include(addrpkg);
        for (auto &subzone : page->subzone_tranveser()) {
            page->create_subzone(&subzone, size_need);
            format_subzone(&subzone, level);
            return &subzone;
        }
    }
    return nullptr;
}
void KernelHeapFormat::PageHead::create_subzone(SubZone *free_sz, size_t size) {
    auto szbuf = *free_sz;
    auto count = ((u8 *)szbuf.end - (u8 *)(&szbuf+1))/size;
    if (count > 0x40) count = 0x40;

    free_sz->objsize = size;
    free_sz->end = (u8 *)(free_sz+1)+size*count;
    free_sz->type = SubZone::Type::inl;

    // free subzone
    new (free_sz->end) SubZone {
        .end = szbuf.end,
    };
}
void KernelHeapFormat::deletezone(KernelHeapFormat::SubZone *zone) {
    zone->objsize = 0; // label of free
}
void KernelHeapFormat::format_subzone(SubZone *sz, u32 level) {
    ObjHeader buf{}, *lastptr = &buf;
    u32 objcount = 0;
    for(u8 *objptr = (u8 *)(sz+1);objptr < sz->end;lastptr = (ObjHeader *)objptr, objptr+=sz->objsize) {
        ((ObjHeader *)objptr)->head_guard = 0;
        lastptr->next = (ObjHeader *)objptr;
        objcount++;
    }
    lastptr->next = sb->free_zones[level].obj_start;
    sb->free_zones[level].obj_start = buf.next;
    sb->free_zones[level].free_count += objcount;
    //cout << "level(C): " << level << ", start: " << sb->free_zones[level].obj_start << '\n';
}
void KernelHeapFormat::format(address_package addrpkg) {
    sb = (SuperBlock *)new (reinterpret_cast<void *>(addrpkg.address.low)) SuperBlock {
        .magic = SuperBlockMagic, .total_pages = addrpkg.len.low
    };
    sb->page_head = (PageHead *) new (sb+1) PageHead { 
        (void *)((addrpkg.address+addrpkg.len).low) 
    };
}
KernelHeapFormat::PageHead *KernelHeapFormat::include(address_package addrpkg) {
    PageHead *page = new (reinterpret_cast<void *>(addrpkg.address.low)) PageHead {
        (void *)((addrpkg.address+addrpkg.len).low)
    };
    sb->page_head = page;
    return page;
}

// === tools ===
bool KernelHeapFormat::SubZone::is_end_dc() {
    return type == Type::end;
}
bool KernelHeapFormat::SubZone::is_free_dc() {
    return objsize == 0;
}
KernelHeapFormat::PageHead::PageHead(void *endaddr):
magic{PageHeadMagic}, page_end{endaddr} {
    auto subzone_baseptr = (SubZone *)(this+1);
    new (subzone_baseptr) SubZone { .end = endaddr, };
}
u32 KernelHeapFormat::PageHead::get_free_size() {
    u32 size = 0;
    for (auto subzone : subzone_tranveser())
    if (subzone.is_free_dc())
    size += ((u8 *)subzone.end - (u8 *)(&subzone+1));

    return size;
}
KernelHeapFormat::PageHead::tran::iterator &
KernelHeapFormat::PageHead::tran::iterator::operator++() {
    if (done == 0) return *this;
    if (ptr->is_end_dc()) done = 1;
    else ptr = static_cast<SubZone *>(ptr->end);
    return *this;
}
int KernelHeapFormat::size_to_level(u32 size) {
    u32 label = 0;
    for (auto l : size_classes) {
        if (l > size) return label;
        label++;
    }
    return (u32)-1;
}
KernelHeapFormat::PageHead *KernelHeapFormat::search_page(void *obj) {
    for (auto &page : rtl::list_tranveser{sb->page_head})
    if (obj > (void *)&page && obj < page.page_end) return &page;
    return nullptr;
}
KernelHeapFormat::SubZone *KernelHeapFormat::search_zone(void *obj, KernelHeapFormat::PageHead *page) {
    for (auto &subzone : page->subzone_tranveser())
    if (obj > &subzone+1 && obj < subzone.end) return &subzone;
    return nullptr;
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
        for(auto addrpkg : physicalpage.aloc(need_page, true))
        box = (ObjHeader *)addrpkg.address.low;

        if(box == (void *)-1) return nullptr;
        box->next = (ObjHeader *)need_page;
        box->head_guard = ObjectSingalGuard;
        //cout << "<addr=" << box+1 << ">\n";
        return box+1;
    }

    auto &objzone = sb->free_zones[level];
    if (objzone.obj_start == nullptr) {
        // need to new subzone
        createzone(level);
        //cout << "level(P): " << level << ", objzone start: " << objzone.obj_start << '\n';
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

    //cout << "<addr=" << box+1 << ">";
    return box+1;
}
void MemoryManager::nopage::dlloc(void *ptr, size_t) {
    auto objhead = static_cast<ObjHeader *>(ptr)-1;
    if (objhead->head_guard == ObjectSingalGuard) {
        objhead->head_guard = 0;
        physicalpage.dloc({(u32)objhead, (u32)objhead->next});
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
void *MemoryManager::inpage::alloc(size_t) { return nullptr; }
void MemoryManager::inpage::dlloc(void *, size_t) {}
MemoryManager::MemoryManager():
reserved_space{._rn{nullptr}} {
#ifdef _DEBUG
cout << "< heap: " << *(void **)(&reserved_space._rn);
#endif
}

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
