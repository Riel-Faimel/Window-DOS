#include "_mm.hpp"
using namespace mem;

#pragma GCC optimize("Os")

PhysicalPage physicalpage;
MemoryManager *global_heap;

KernelHeapFormat::KernelHeapFormat(SuperBlock *page_base){
    if (page_base == nullptr) {
        auto pkg = *physicalpage.aloc(PageNum{4096u}, true).begin();
        format(pkg);
    } else if (page_base->magic == SuperBlockMagic) {
        sb = page_base;
    }
}
KernelHeapFormat::SubZone *KernelHeapFormat::createzone(u32 level) {
    u32 size_need = static_cast<u32>(slot_size(level));
    //kout << "size need = " << size_need << '\n';

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
        u32 available = (u32)subzone.end - (u32)(&subzone+1);
        if (available >= size_need + (u32)sizeof(SubZone)) { // room for objects + terminator
            if (page.create_subzone(&subzone, size_need)) {
                format_subzone(&subzone, level);
                return &subzone;
            }
        }
    }
    // not found
    for (auto addrpkg : physicalpage.aloc(PageNum{512}, true)) {
        if (addrpkg.base == Address{0u}) addrpkg = *physicalpage.aloc(PageNum{2}, true).begin();
        auto page = include(addrpkg);
        for (auto &subzone : page->subzone_tranveser()) {
            if (page->create_subzone(&subzone, size_need)) {
                format_subzone(&subzone, level);
                return &subzone;
            }
        }
    }
    return nullptr;
}
bool KernelHeapFormat::PageHead::create_subzone(SubZone *free_sz, size_t size) {
    void *old_end = free_sz->end;
    size_t available = (u8 *)old_end - (u8 *)(free_sz + 1);
    size_t count = available / size;
    if (count > 0x40) count = 0x40;
    // Leave room for the trailing SubZone descriptor.
    while (count && count * size + sizeof(SubZone) > available) --count;
    if (count == 0) return false;

    free_sz->objsize = (u16)size;
    free_sz->end = (u8 *)(free_sz + 1) + size * count;
    free_sz->type = SubZone::Type::inl;

    // free subzone
    new (free_sz->end) SubZone {
        .end = old_end,
    };
    return true;
}
void KernelHeapFormat::deletezone(KernelHeapFormat::SubZone *zone) {
    zone->objsize = 0; // label of free
}
void KernelHeapFormat::format_subzone(SubZone *sz, u32 level) {
    ObjHeader buf{}, *lastptr = &buf;
    BoxNum objcount = 0;
    for(
        u8 *objptr = (u8 *)(sz+1);objptr < sz->end;
        lastptr = (ObjHeader *)objptr, objptr+=sz->objsize
    ) {
        ((ObjHeader *)objptr)->head_guard = 0;
        lastptr->next = (ObjHeader *)objptr;
        objcount++;
    }
    lastptr->next = sb->free_zones[level].obj_start;
    sb->free_zones[level].obj_start = buf.next;
    sb->free_zones[level].free_count += objcount;
    //kout << "level(C): " << level << ", start: " << sb->free_zones[level].obj_start << '\n';
}
void KernelHeapFormat::format(PagePackage addrpkg) {
    sb = (SuperBlock *)new (Pointer<ObjHeader>{addrpkg.base}) SuperBlock {
        .magic = SuperBlockMagic, .total_pages = addrpkg.pages
    };
    ASMLabelMark(enter)
    sb->page_head = (PageHead *) new (sb+1) PageHead {
        Pointer<int>(addrpkg.base+static_cast<ByteNum>(addrpkg.pages))
    };
}
KernelHeapFormat::PageHead *KernelHeapFormat::include(PagePackage addrpkg) {
    PageHead *page = new (static_cast<void *>(Pointer<int>{addrpkg.base})) PageHead {
        (void *)(Pointer<int>{addrpkg.base+static_cast<ByteNum>(addrpkg.pages)})
    };
    page->next = sb->page_head;
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
        if (l >= size) return label;
        label++;
    }
    return -1;
}

void KernelHeapFormat::heap_status(
    const SuperBlock *sb,
    size_t &total, size_t &alloced, size_t &used, size_t &free
) {
    total = 0;
    alloced = 0;
    used = 0;
    free = 0;
    if (!sb) return;

    for (auto &page : rtl::list_tranveser{sb->page_head}) {
        bool first = true;
        for (auto &subzone : page.subzone_tranveser()) {
            // The initial whole-page free zone is a Type::end descriptor too.
            if (first) {
                first = false;
                if (subzone.is_end_dc() && subzone.objsize == 0) {
                    if (!subzone.end) continue;
                    const auto begin = reinterpret_cast<u8 *>(&subzone + 1);
                    const auto end = reinterpret_cast<u8 *>(subzone.end);
                    if (end > begin) {
                        const size_t bytes = static_cast<size_t>(end - begin);
                        total += bytes;
                        free += bytes;
                    }
                    continue;
                }
            }

            // The trailing descriptor is only a terminator, not free memory.
            if (subzone.is_end_dc()) continue;
            if (!subzone.end) continue;

            const auto begin = reinterpret_cast<u8 *>(&subzone + 1);
            const auto end = reinterpret_cast<u8 *>(subzone.end);
            if (end <= begin) continue;

            // Unformatted/free subzone: its whole object area is free.
            if (subzone.objsize == 0) {
                const size_t bytes = static_cast<size_t>(end - begin);
                total += bytes;
                free += bytes;
                continue;
            }

            // Formatted subzone: walk its slots and read the per-object guard.
            int level = -1;
            for (u32 l = 0; l < SizeClassCount; l++) {
                if (slot_size(l) == subzone.objsize) {
                    level = static_cast<int>(l);
                    break;
                }
            }
            if (level < 0) continue;

            const size_t step = subzone.objsize;
            for (u8 *obj = begin; obj + step <= end; obj += step) {
                total += step;
                const auto *head = reinterpret_cast<const ObjHeader *>(obj);
                if (head->head_guard == ObjectHeadGuard) {
                    alloced += step;
                    used += size_classes[level];
                } else {
                    free += step;
                }
            }
        }
    }

    // Keep the stated invariants even if a page/subzone was corrupted.
    if (free > total) free = total;
    if (alloced > total - free) alloced = total - free;
    if (used > alloced) used = alloced;
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


MemoryManager::nopage::nopage(SuperBlock* page_base):
Allocable{page_base} {}

MemSpace MemoryManager::nopage::alloc() { return {}; }
void MemoryManager::nopage::dlloc(MemSpace) {}

void *MemoryManager::nopage::alloc(MemSpace, size_t s, MemAttr) { return alloc(s); }
void MemoryManager::nopage::dlloc(MemSpace, void *ptr, size_t) { dlloc(ptr, -1); }

__attribute__((optimize("O3"), hot))
void *MemoryManager::nopage::alloc(size_t size) {
    auto level = size_to_level(size);
    if (level == -1) {
        // alloc as page
        for(auto [base, pages] : physicalpage.aloc(
            static_cast<PageNum>(ByteNum{size + sizeof(ObjHeader)}), true
        )) {
            auto box = Pointer<ObjHeader>{base};

            if(box == (void *)-1) return nullptr;
            box->pages = pages;
            box->head_guard = ObjectSingalGuard;
            return box+1;
        }
    } else {
        auto &objzone = sb->free_zones[level];
        if (objzone.obj_start == nullptr) {
            // need to new subzone
            if (!createzone(level)) return nullptr;
        }

        // alloc
        auto box = objzone.obj_start;
        if (!box) return nullptr;
        objzone.free_count--;
        objzone.obj_start = box->next;

        // set head and guard; level is read by dlloc, so it goes in after the
        // header construction (the header's union starts as next=nullptr).
        new (box) ObjHeader{.next = nullptr, .head_guard = ObjectHeadGuard};
        box->level = level;
        *reinterpret_cast<u16 *>(
            reinterpret_cast<u8 *>(box + 1) + size_classes[level]
        ) = ObjectTailGuard;

        // init done return
        return box+1;
    }
    return nullptr;
}
__attribute__((optimize("O3"), hot))
void MemoryManager::nopage::dlloc(void *ptr, size_t) {
    auto objhead = static_cast<ObjHeader *>(ptr)-1;
    if (objhead->head_guard == ObjectSingalGuard) {
        objhead->head_guard = 0;
        physicalpage.dloc({Address{objhead}, static_cast<PageNum>(objhead->pages)});
    } else if (objhead->head_guard == ObjectHeadGuard) {
        objhead->head_guard = 0;
        auto &zone = sb->free_zones[objhead->level];
        objhead->next = zone.obj_start;
        zone.obj_start = objhead;
        zone.free_count++;
    }
}

MemoryManager::MemoryManager():
defaul{nullptr}, al{&defaul} {
    global_heap = this;
    kout << "[MM] heap: " << defaul.getsbp() << "\n";
}

MemoryManager::MemStatus MemoryManager::status() {
    MemStatus st{};
    if (!al) return st;

    KernelHeapFormat::heap_status(
        al->getsbp(),
        st.total_mem, st.alloced_mem, st.used_mem, st.free_mem
    );
    return st;
}
void MemoryManager::set_allocator(Allocable *(*newer)(KernelHeapFormat::SuperBlock *)) {
    auto newal = newer(al->getsbp());
    delete al;
    al = newal;
}

//======

void *operator new(size_t size) { return global_heap->allocator()->alloc(size); }
void operator delete(void *ptr) { return global_heap->allocator()->dlloc(ptr); }
void operator delete(void *ptr, size_t size) { return global_heap->allocator()->dlloc(ptr, size); }

void *operator new[](size_t size) { return global_heap->allocator()->alloc(size); }
void operator delete[](void *ptr) { return global_heap->allocator()->dlloc(ptr); }
void operator delete[](void *ptr, size_t size) { return global_heap->allocator()->dlloc(ptr, size); }

/**
 *     ^
 *    | |
 *    | |
 *  ( ) ( )
 */
