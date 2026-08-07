#include "_mm.hpp"

MemoryManager::allocable *allocator;
PhysicalPage* physicalpage;
PhysicalPage::mem_list *PhysicalPage::mem_list_root = __boot_loader_end;

PhysicalPage::PhysicalPage(){
    mem_list_root[0] = {
        .mem = {},
        .next = nullptr,
        .exist = 255, .r1 = 0, 
        .r2 = 0, .r3 = 0,
    };
    u64 total_byte_num{};

    for (unsigned i = 0;i < boot_infomation->mmap_size;i++)
    if(mmap_struc_base[i].type == 1) {
        u64 base{mmap_struc_base[i].base_high, mmap_struc_base[i].base_low};
        u64 len{mmap_struc_base[i].len_high, mmap_struc_base[i].len_low};
        if (base < 0x100000) {
            u64 top = base + len;
            if (top < 0x100000) continue;

            mem_list_root[i].next = &mem_list_root[i+1];
            mem_list_root[i+1] = {
                .mem = {
                    .address{0x100000},
                    .len = top - 0x100000,
                },
                .next = nullptr,
                .exist = 1,
                .r1 = 0, .r2 = 0, .r3 = 0,
            };
        } 
        else {
            mem_list_root[i].next = &mem_list_root[i+1];
            mem_list_root[i+1] = {
                .mem = {base, len},
                .next = nullptr,
                .exist = 1,
                .r1 = 0, .r2 = 0, .r3 = 0,
            };
        }

        auto re = total_byte_num + mem_list_root[i].mem.len;
    }

    /**
     * for root node, address come to total bytes
     * len for total free bytes
     */
    mem_list_root[0].mem.len = total_byte_num; // all free
    mem_list_root[0].mem.address = total_byte_num;
    physicalpage = this;
}
PhysicalPage::~PhysicalPage() {}
PhysicalPage::address_generator PhysicalPage::aloc(size_t page_nums, bool need_continuous) {
    return address_generator{page_nums, need_continuous, this};
}
unsigned PhysicalPage::dloc(address_package addrpkg) {
    auto s = findspace();
    if (!s) {
        neaten();
        s = findspace();
        if (!s) return -1;
    }
    mem_list *new_node = new (s) mem_list{
        .mem = addrpkg,
        .next = mem_list_root[0].next,
        .exist = 1,
        .r1 = 0, .r2 = 0, .r3 = 0,
    };
    mem_list_root[0].next = new_node;
    return 0;
}
PhysicalPage::mem_list *PhysicalPage::findspace() {
    for (auto memnode : *this) {
        if (memnode->exist == 0) return memnode;
    }
    return nullptr;
}
unsigned PhysicalPage::neaten() {
    ;
}
PhysicalPage::Iterator PhysicalPage::begin() {
    return Iterator{};
}
PhysicalPage::Iterator PhysicalPage::end() {
    return Iterator{};
}
PhysicalPage::mem_list *PhysicalPage::Iterator::operator*() {
    return ::PhysicalPage::mem_list_root+count;
}
bool PhysicalPage::Iterator::operator!=(PhysicalPage::Iterator &) { return !done; }
PhysicalPage::Iterator &PhysicalPage::Iterator::operator++() {
    if (count * sizeof(mem_list) >= 0x9fff0 - StackSize - (u32)::PhysicalPage::mem_list_root) {
        done = true;
    } else {
        count ++;
    }
}

PhysicalPage::address_generator::address_generator(size_t pn, bool nc, PhysicalPage* mm):
pageneedednums{pn}, need_continuous{nc}, memmgrptr{mm}{
    // stop yield
}
PhysicalPage::address_generator::~address_generator() {
    // resume yield
}
PhysicalPage::address_generator::iterator PhysicalPage::address_generator::begin() {
    return {this};
}
PhysicalPage::address_generator::iterator PhysicalPage::address_generator::end() const {
    return {nullptr};
}

PhysicalPage::address_generator::iterator::iterator(PhysicalPage::address_generator *ptr):
done{false}, count{1}, conditions{ptr}{
    if (
        ::PhysicalPage::mem_list_root[0].mem.len / 4096 > conditions->pageneedednums
    ) {
        no_mem = true;
        done = true;
    }
}
address_package PhysicalPage::address_generator::iterator::operator*() {
    if (no_mem) return {.address = -1};
    if (conditions->need_continuous) {
        for (auto mem : *conditions->memmgrptr) {
            if (mem->exist) {
                if (
                    auto p = mem->mem.address/4096;
                    p >= conditions->pageneedednums
                ) {
                    auto r = p - conditions->pageneedednums;
                    if (r > p) {}
                    mem->mem.address = r*4096;
                }
            }
        }
    }
    else {
        ;
    }
}
bool PhysicalPage::address_generator::iterator::operator!=(PhysicalPage::address_generator::iterator &){
    return !done;
}
PhysicalPage::address_generator::iterator &PhysicalPage::address_generator::iterator::operator++() {
    if (
        conditions->pageneedednums && // need mem
        ::PhysicalPage::mem_list_root[count].next &&
        ::PhysicalPage::mem_list_root[count].next->exist == 1 // have mem
    ) { count++; }
    else { done = true; }
    return *this;
}

//======

KernelHeapFormat::KernelHeapFormat(void *page_base): sb{nullptr}{
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

    head *hptr = new (reinterpret_cast<void *>(addrpkg.address.low)) head {};
    hptr->sb.magic = SuperBlockMagic;
    hptr->ph.magic = PageHeadMagic;
    hptr->sb.total_pages = addrpkg.len.low;
    hptr->ph.page_count = addrpkg.len.low;
    hptr->sb.page_head = &(hptr->ph);
}
void KernelHeapFormat::include(address_package addrpkg) {
    ;
}
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
u32 KernelHeapFormat::size_to_level(u32 size) {
    u32 label = 0;
    for (auto l : size_classes) {
        if (l > size) return label;
        label++;
    }
    return (u32)-1;
}

MemoryManager::allocable::allocable() {
    allocator = this;
}

MemoryManager::nopage::nopage(void* page_base):
KernelHeapFormat{page_base} {
    if (page_base == nullptr) {
        for (auto pkg : physicalpage->aloc(4096, true)) {
            //only one time
            format(pkg);
        }
    }
}
void *MemoryManager::nopage::alloc(size_t size) {
    KernelHeapFormat::ObjHeader *ptr;
    auto level = size_to_level(size);
    if (level == (u32)-1) {
        // alloc as page
    }

    auto zone_start = sb->free_zones[level];
    if (zone_start == nullptr) {
        // need to init subzone level
    } else while (zone_start->free_count == 0) {
        if (zone_start->next == nullptr) {
            // need to new subzone
            break;
        }
        zone_start = zone_start->next;
    }

    ObjHeader *box = findspace(zone_start);
    // set head and guard
    new (box) ObjHeader{.zone = zone_start, .head_guard = ObjectHeadGuard};
    *reinterpret_cast<u16 *>(reinterpret_cast<u8 *>(box + 1) + size_classes[level]) = ObjectTailGuard;

    return box+1;
}
void MemoryManager::nopage::dlloc(void *, size_t) {}

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
