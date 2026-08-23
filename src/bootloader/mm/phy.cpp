#include "_phy.hpp"

PhysicalPage physicalpage;
PhysicalPage::mem_list *__window_dos_loader_struct_0x1::start = (PhysicalPage::mem_list *)0x27000;
atomic<bool> physical_memory_lock{};

PhysicalPage::PhysicalPage(){
    mem_list_root[0] = {
        .mem = {u64{0}, u64{0}},
        .next = &(mem_list_root[1]),
        .exist = 255, 
        .r1 = 0, .r2 = 0, .r3 = 0,
    };
    u64 total_byte_num{};
    unsigned node_index = 1;

    for (unsigned i = 0;i < boot_infomation->mmap_size;i++)
    if(mmap_struc_base[i].type == 1) {
        u64 base{mmap_struc_base[i].base_high, mmap_struc_base[i].base_low};
        u64 len{mmap_struc_base[i].len_high, mmap_struc_base[i].len_low};

        if (base < 0x100000) {
            u64 top = base + len;
            if (top < 0x100000) continue;

            base = u64{0x100000};
            len = top - 0x100000;
        } 

        mem_list_root[node_index-1].next = &(mem_list_root[node_index]);
        mem_list_root[node_index] = {
            .mem = {base, len},
            .next = nullptr,
            .exist = 1,
            .r1 = 0, .r2 = 0, .r3 = 0,
        };

        total_byte_num += len;
        node_index++;
    }

    /**
     * for root node, address come to total bytes
     * len for total free bytes
     */
    mem_list_root[0].mem.len = total_byte_num; // all free
    mem_list_root[0].mem.address = total_byte_num;
}
PhysicalPage::~PhysicalPage() {}
PhysicalPage::address_generator PhysicalPage::aloc(size_t page_nums, bool need_continuous) {
    if (!physical_memory_lock.try_lock());
    return address_generator{page_nums, need_continuous};
}
unsigned PhysicalPage::dloc(address_package addrpkg) {
    auto s = mem_list_root.findspace();
    if (!s) {
        neaten();
        s = mem_list_root.findspace();
        if (!s) return -1;
    }
    mem_list *new_node = new (s) mem_list{
        .mem = { .address = addrpkg.address, .len = addrpkg.len*4096 },
        .next = mem_list_root[0].next,
        .exist = 1,
        .r1 = 0, .r2 = 0, .r3 = 0,
    };
    mem_list_root[0].next = new_node;
    return 0;
}
constexpr PhysicalPage::mem_list *__window_dos_loader_struct_0x1::findspace() {
    for (auto memnode : as_arr()) {
        if (memnode->exist == 0) return memnode;
    }
    return nullptr;
}
unsigned PhysicalPage::neaten() {
    ;
}

__window_dos_loader_struct_0x1::array::Iterator&
__window_dos_loader_struct_0x1::array::Iterator::operator++() {
    if (count * sizeof(mem_list) >= 0x9FC00 - StackSize - (u32)start) {
        done = true;
    } else { count ++; }
    return *this;
}

//======

PhysicalPage::address_generator::address_generator(size_t pn, bool nc):
pageneedednums{pn}, need_continuous{nc}{
    // stop yield
}
PhysicalPage::address_generator::~address_generator() {
    // resume yield
    physical_memory_lock.unlock();
}

PhysicalPage::address_generator::iterator::iterator(PhysicalPage::address_generator *ptr):
conditions{ptr}, thisptr{*mem_list_root}, lastptr{mem_list_root.start}, no_mem{false}, done{false}{
    if (mem_list_root[0].mem.len / 4096 < conditions->pageneedednums) 
    { done = true; }
}
address_package PhysicalPage::address_generator::iterator::operator*() {
    u64 alloc_addr, alloc_page;
    if (no_mem) {
        done = true;
        return { .address = u64{0}, .len = u64{0}, };
    }
    if (conditions->need_continuous) {

for(char i = 0;i < 2;i++){
    // try 2
    for (auto &node : mem_list_root.as_list()) {
        alloc_page = u64{conditions->pageneedednums};
        auto total_pages = node.mem.len / 4096;
        if (total_pages >= alloc_page) {
            alloc_addr = node.mem.address;
            
            node.mem.address += alloc_page * 4096;
            node.mem.len -= alloc_page * 4096;
            
            done = true;
            return { .address = alloc_addr, .len = alloc_page, };
        }
    }
    physicalpage.neaten();
}
    } else {

auto node_pagenum = thisptr->mem.len/4096;
if (node_pagenum > conditions->pageneedednums) {
    u64 need_bytes {conditions->pageneedednums};
    need_bytes = need_bytes * 4096;

    alloc_addr = thisptr->mem.address;
    thisptr->mem.address += need_bytes;
    thisptr->mem.len -= need_bytes;
    alloc_page = u64{conditions->pageneedednums};

    done = true;
} else {
    conditions->pageneedednums -= node_pagenum;
    lastptr->next = thisptr->next;
    thisptr->exist = 0;
    alloc_addr = thisptr->mem.address;
    alloc_page = node_pagenum;
}
    }
    return { .address = alloc_addr, .len = alloc_page, };
}

/**
 *     ^
 *    | |
 *    | |
 *  ( ) ( )
 */