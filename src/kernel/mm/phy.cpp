#pragma GCC optimize("Os")
#include "_phy.hpp"

using namespace mem;

namespace {
size_t byte_num_to_size(const ByteNum &bytes) {
    const u64 value = static_cast<u64>(bytes);
    if (value.high) return static_cast<size_t>(-1);
    return static_cast<size_t>(value.low);
}
}

mem_list *__window_dos_loader_struct_0x1::start = __kernel_end;
atomic<bool> physical_memory_lock{};

PhysicalPage::PhysicalPage(){
    mem_list_root[0] = {
        .mem = {Address{0u}, ByteNum{}},
        .next = &(mem_list_root[1]),
        .exist = 255, 
        .r1 = 0, .r2 = 0, .r3 = 0,
    };
    ByteNum total_byte_num{};
    unsigned node_index = 1;

    for (unsigned i = 0;i < arch::boot_infomation->mmap_size;i++)
    if(mmap_struc_base[i].type == 1) {
        Address base{mmap_struc_base[i].base_high, mmap_struc_base[i].base_low};
        ByteNum len{mmap_struc_base[i].len_high, mmap_struc_base[i].len_low};

        constexpr Address allocable_mm_base{0x100000u};
        if (base < allocable_mm_base) {
            auto top = base + len;
            if (top < allocable_mm_base) continue;

            base = allocable_mm_base;
            len = top - allocable_mm_base;
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
    mem_list_root[0].mem.address = Address{0u} + total_byte_num;
    total_bytes = byte_num_to_size(total_byte_num);
}
PhysicalPage::~PhysicalPage() {}

PhysicalPage::PageStatus PhysicalPage::status() {
    PageStatus st{};
    st.total_mem = total_bytes;

    size_t free = 0;
    for (auto &node : mem_list_root.as_list()) {
        // exist == 255 is the root/accounting node, == 0 is a consumed node.
        if (node.exist != 1) continue;
        free += byte_num_to_size(node.mem.len);
    }

    st.free_mem = free > st.total_mem ? st.total_mem : free;
    return st;
}
PhysicalPage::address_generator PhysicalPage::aloc(PageNum page_nums, bool need_continuous) {
    //if (!physical_memory_lock.try_lock());
    return address_generator{page_nums, need_continuous};
}
HotFunc unsigned PhysicalPage::dloc(PagePackage addrpkg) {
    auto s = mem_list_root.findspace();
    if (!s) {
        neaten();
        s = mem_list_root.findspace();
        if (!s) return -1;
    }
    mem_list *new_node = new (s) mem_list{
        .mem = { .address = addrpkg.base, .len = static_cast<ByteNum>(addrpkg.pages) },
        .next = mem_list_root[0].next,
        .exist = 1,
        .r1 = 0, .r2 = 0, .r3 = 0,
    };
    mem_list_root[0].next = new_node;
    return 0;
}
constexpr mem_list *__window_dos_loader_struct_0x1::findspace() {
    for (auto memnode : as_arr()) {
        if (memnode->exist == 0) return memnode;
    }
    return nullptr;
}

unsigned PhysicalPage::neaten() {
    auto cursor = *mem_list_root;
    auto [left_list, left_len] = branch_sort(cursor, 4);
    auto left_root = left_list;
    while (cursor != nullptr) {
        auto [right_list, right_len] = right_sort(cursor, left_len);
        auto [new_left_list, len] = merge(left_root, left_len, right_list, right_len);
        left_root = new_left_list;
        left_len = len;
    }
    mem_list_root.start = left_root;
    return 0;
}

__window_dos_loader_struct_0x1::array::iterator&
__window_dos_loader_struct_0x1::array::iterator::operator++() {
    if (count * sizeof(mem_list) >= 0x9FC00 - StackSize - (u32)start) {
        done = true;
    } else { count ++; }
    return *this;
}

//======

PhysicalPage::address_generator::address_generator(PageNum pn, bool nc):
needpages{pn}, need_continuous{nc}{
    // stop yield
}
PhysicalPage::address_generator::~address_generator() {
    // resume yield
    // physical_memory_lock.unlock();
}

PhysicalPage::address_generator::iterator::iterator(PhysicalPage::address_generator *ptr):
conditions{ptr}, thisptr{*mem_list_root}, lastptr{mem_list_root.start}, no_mem{false}, done{false}{
    if (static_cast<PageNum>(mem_list_root[0].mem.len) < conditions->needpages) 
    { done = true; }
}
HotFunc PagePackage PhysicalPage::address_generator::iterator::operator*() {
    Address alloc_addr{0u};
    PageNum alloc_page;
    if (no_mem) {
        done = true;
        return { .base = Address{0u}, .pages = PageNum{}, };
    }
    if (conditions->need_continuous) {

for(char i = 0;i < 2;i++){
    // try 2
    for (auto &node : mem_list_root.as_list()) {
        alloc_page = conditions->needpages;
        auto total_pages = static_cast<PageNum>(node.mem.len);
        if (total_pages >= alloc_page) {
            alloc_addr = node.mem.address;
            
            node.mem.address += static_cast<ByteNum>(alloc_page);
            node.mem.len -= static_cast<ByteNum>(alloc_page);
            
            done = true;
            return { .base = alloc_addr, .pages = alloc_page, };
        }
    }
    physicalpage.neaten();
}
    } else {

auto node_pagenum = static_cast<PageNum>(thisptr->mem.len);
if (node_pagenum > conditions->needpages) {
    auto need_bytes = static_cast<ByteNum>(conditions->needpages);

    alloc_addr = thisptr->mem.address;
    thisptr->mem.address += need_bytes;
    thisptr->mem.len -= need_bytes;
    alloc_page = conditions->needpages;

    done = true;
} else {
    conditions->needpages -= node_pagenum;
    lastptr->next = thisptr->next;
    thisptr->exist = 0;
    alloc_addr = thisptr->mem.address;
    alloc_page = node_pagenum;
}
    }
    return { .base = alloc_addr, .pages = alloc_page, };
}

/**
 *     ^
 *    | |
 *    | |
 *  ( ) ( )
 */