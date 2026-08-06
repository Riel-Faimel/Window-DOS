#include "_mm.hpp"

MemoryManager::allocable *allocator;
PhysicalPage* physicalpage;

PhysicalPage::PhysicalPage():
mem_list_root{__boot_loader_end}{
    mem_list_root[0] = {
        .mem = {},
        .next = nullptr,
        .exist = 255, .r1 = 0, 
        .r2 = 0, .r3 = 0,
    };
    struct {
        u32 h = 0;
        u32 l = 0;
    } total_page_num;

    for (unsigned i = 0;i < boot_infomation->mmap_size;i++)
    if(mmap_struc_base[i].type == 1) {
        if (mmap_struc_base[i].base_low < 0x100000 && mmap_struc_base[i].base_high == 0) {
            u32 top_low = mmap_struc_base[i].len_low + mmap_struc_base[i].base_low, top_high = 0;
            if (top_low < mmap_struc_base[i].len_low || top_low < mmap_struc_base[i].base_low) {
                top_high++;
            }
            top_high += mmap_struc_base[i].len_high + mmap_struc_base[i].base_high;
            if (top_high < 0x100000) continue;

            mem_list_root[i].next = &mem_list_root[i+1];
            mem_list_root[i+1] = {
                .mem = {
                    .address_high = 0,
                    .address_low = 0x100000,
                    .len_high = (mmap_struc_base[i].len_high),
                    .len_low = ((mmap_struc_base[i].len_low - 0x100000) + mmap_struc_base[i].base_low),
                },
                .next = nullptr,
                .exist = 1,
                .r1 = 0, .r2 = 0, .r3 = 0,
            };
        } 
        else {
            mem_list_root[i].next = &mem_list_root[i+1];
            mem_list_root[i+1] = {
                .mem = {
                    .address_high = mmap_struc_base[i].base_high,
                    .address_low = mmap_struc_base[i].base_low,
                    .len_high = (mmap_struc_base[i].len_high),
                    .len_low = (mmap_struc_base[i].len_low),
                },
                .next = nullptr,
                .exist = 1,
                .r1 = 0, .r2 = 0, .r3 = 0,
            };
        }

        auto re = total_page_num.l + mem_list_root[i].mem.len_low;
        if (re < total_page_num.l || re < mem_list_root[i].mem.len_low) {
            total_page_num.h++;
        }
        total_page_num.l = re;
        total_page_num.h += mem_list_root[i].mem.len_high;
    }

    /**
     * for root node, address come to total bytes
     * len for total free bytes
     */
    mem_list_root[0].mem.len_high = total_page_num.h;
    mem_list_root[0].mem.len_low = total_page_num.l;
    mem_list_root[0].mem.address_high = total_page_num.h;
    mem_list_root[0].mem.address_low = total_page_num.l;
    physicalpage = this;
}
PhysicalPage::~PhysicalPage() {}
PhysicalPage::address_generator PhysicalPage::aloc(size_t page_nums, bool need_continuous) {
    return address_generator{page_nums, need_continuous, this};
}
unsigned PhysicalPage::dloc(address_package addrpkg) {
    mem_list *new_node = new (findspace()) mem_list{
        .mem = addrpkg,
        .next = mem_list_root[0].next,
        .exist = 1,
        .r1 = 0, .r2 = 0, .r3 = 0,
    };
    mem_list_root[0].next = new_node;
}
PhysicalPage::mem_list *PhysicalPage::findspace() {
    for (unsigned i = 0;reinterpret_cast<u32>(mem_list_root + i) < (0x9FFF0 - 4096);i++) {
        if (mem_list_root->exist == 0) return mem_list_root+i;
    }
    return nullptr;
}

//======

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

//======

PhysicalPage::address_generator::iterator::iterator(PhysicalPage::address_generator *ptr):
done{false}, count{1}, conditions{ptr}{
    if (
        conditions->memmgrptr->mem_list_root[0].mem.len_high || // 64 always larger then 32
        conditions->memmgrptr->mem_list_root[0].mem.len_low / 4096 > conditions->pageneedednums
    ) {
        no_mem = true;
        done = true;
    }
}
address_package PhysicalPage::address_generator::iterator::operator*() {
    if (no_mem) return {.address_high = (u32)-1, .address_low = (u32)-1};
    if (conditions->need_continuous) {
        ;
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
        conditions->memmgrptr->mem_list_root[count].next &&
        conditions->memmgrptr->mem_list_root[count].next->exist == 1 // have mem
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

    head *hptr = new (reinterpret_cast<void *>(addrpkg.address_low)) head {};
    hptr->sb.magic = SuperBlockMagic;
    hptr->ph.magic = PageHeadMagic;
    hptr->sb.total_pages = addrpkg.len_low;
    hptr->ph.page_count = addrpkg.len_low;
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
    }
    while (zone_start->free_count == 0) {
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
