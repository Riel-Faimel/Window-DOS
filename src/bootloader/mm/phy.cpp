#include "_phy.hpp"

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

rtl::pair<PhysicalPage::mem_list *, size_t> right_sort(PhysicalPage::mem_list *&last_tail, size_t len_max);
inline void append_or_merge(PhysicalPage::mem_list *&tail, PhysicalPage::mem_list *chosen, size_t& new_len) {
    if (tail->mem.address + tail->mem.len == chosen->mem.address) {
        tail->mem.len += chosen->mem.len;
        tail->next = chosen->next;
        new_len--;
    } else {
        tail->next = chosen;
        tail = tail->next;
    }
}
inline rtl::pair<PhysicalPage::mem_list*, size_t> merge(
    PhysicalPage::mem_list* llist, size_t llen,
    PhysicalPage::mem_list* rlist, size_t rlen
) {
    if (llist == nullptr) llen = 0;
    if (rlist == nullptr) rlen = 0;
    PhysicalPage::mem_list* result = nullptr;
    size_t i = 0, j = 0;
    size_t new_len = llen + rlen;

    if (llen == 0) {
        result = rlist;
        rlist = rlist->next;
        j++;
    } else if (rlen == 0) {
        result = llist;
        llist = llist->next;
        i++;
    } else if (llist->mem.address <= rlist->mem.address) {
        result = llist;
        llist = llist->next;
        i++;
    } else {
        result = rlist;
        rlist = rlist->next;
        j++;
    }
    auto tail = result;

    while (i < llen && j < rlen) {
        PhysicalPage::mem_list* chosen;
        if (llist->mem.address <= rlist->mem.address) {
            chosen = llist;
            llist = llist->next;
            i++;
        } else {
            chosen = rlist;
            rlist = rlist->next;
            j++;
        }
        append_or_merge(tail, chosen, new_len);
    }
    while (i < llen) {
        auto chosen = llist;
        llist = llist->next;
        i++;
        append_or_merge(tail, chosen, new_len);
    }
    while (j < rlen) {
        auto chosen = rlist;
        rlist = rlist->next;
        j++;
        append_or_merge(tail, chosen, new_len);
    }

    return {result, new_len};
}
inline void cmp_swap(PhysicalPage::mem_list*& a, PhysicalPage::mem_list*& b) {
    if (a->mem.address > b->mem.address) {
        auto tmp = a; a = b; b = tmp;
    }
}
inline rtl::pair<PhysicalPage::mem_list*, size_t> branch_sort(
    PhysicalPage::mem_list *&last_tail, size_t len_max
) {
    if (last_tail == nullptr) return {nullptr, 0};
    
    PhysicalPage::mem_list* nodes[4];
    size_t n = 0;
    while (last_tail != nullptr && n < len_max) {
        nodes[n++] = last_tail;
        last_tail = last_tail->next;
    }
    
    switch (n) {
    case 0: return {nullptr, 0};
    case 1: break;
    case 2:
        cmp_swap(nodes[0], nodes[1]);
        break;
    case 3:
        cmp_swap(nodes[0], nodes[1]);
        cmp_swap(nodes[1], nodes[2]);
        cmp_swap(nodes[0], nodes[1]);
        break;
    case 4:
        cmp_swap(nodes[0], nodes[1]);
        cmp_swap(nodes[2], nodes[3]);
        cmp_swap(nodes[0], nodes[2]);
        cmp_swap(nodes[1], nodes[3]);
        cmp_swap(nodes[1], nodes[2]);
        break;
    }
    
    size_t result_len = n;
    size_t tail_idx = 0;
    
    for (size_t i = 1; i < n; i++) {
        if (nodes[tail_idx]->mem.address + nodes[tail_idx]->mem.len == nodes[i]->mem.address) {
            nodes[tail_idx]->mem.len += nodes[i]->mem.len;
            nodes[tail_idx]->next = nodes[i]->next;
            result_len--;
        } else {
            nodes[tail_idx]->next = nodes[i];
            tail_idx = i;
        }
    }
    
    return {nodes[0], result_len};
}
inline rtl::pair<PhysicalPage::mem_list*, size_t> insert_sort(
    PhysicalPage::mem_list *&last_tail, size_t len_max
) {
    if (last_tail == nullptr) return {nullptr, 0};
    
    PhysicalPage::mem_list* nodes[16];
    size_t n = 0;
    while (last_tail != nullptr && n < len_max) {
        nodes[n++] = last_tail;
        last_tail = last_tail->next;
    }
    
    PhysicalPage::mem_list* sorted[16];
    size_t sorted_len = 0;
    
    for (size_t k = 0; k < n; k++) {
        auto node = nodes[k];
        size_t pos = 0;
        while (pos < sorted_len && sorted[pos]->mem.address < node->mem.address) {
            pos++;
        }
        
        if (pos > 0 && sorted[pos-1]->mem.address + sorted[pos-1]->mem.len == node->mem.address) {
            sorted[pos-1]->mem.len += node->mem.len;
            if (pos < sorted_len && sorted[pos-1]->mem.address + sorted[pos-1]->mem.len == sorted[pos]->mem.address) {
                sorted[pos-1]->mem.len += sorted[pos]->mem.len;
                for (size_t t = pos; t + 1 < sorted_len; t++) {
                    sorted[t] = sorted[t+1];
                }
                sorted_len--;
            }
        }
        else if (pos < sorted_len && node->mem.address + node->mem.len == sorted[pos]->mem.address) {
            sorted[pos]->mem.address = node->mem.address;
            sorted[pos]->mem.len += node->mem.len;
            if (pos > 0 && sorted[pos-1]->mem.address + sorted[pos-1]->mem.len == sorted[pos]->mem.address) {
                sorted[pos-1]->mem.len += sorted[pos]->mem.len;
                for (size_t t = pos; t + 1 < sorted_len; t++) {
                    sorted[t] = sorted[t+1];
                }
                sorted_len--;
            }
        }
        else {
            for (size_t t = sorted_len; t > pos; t--) {
                sorted[t] = sorted[t-1];
            }
            sorted[pos] = node;
            sorted_len++;
        }
    }
    
    for (size_t k = 0; k + 1 < sorted_len; k++) {
        sorted[k]->next = sorted[k+1];
    }
    
    return {sorted[0], sorted_len};
}
inline auto merge_sort(PhysicalPage::mem_list *&last_tail, size_t len_max) {
    auto left_len = len_max / 2, right_len = len_max-left_len;

    auto [llist, llen] = right_sort(last_tail, left_len);
    auto [rlist, rlen] = right_sort(last_tail, right_len);
    return merge(llist, llen, rlist, rlen);
}
/**
 * output: {result list, result lenght}
 * it push cursor by branch/insert sort
 */
rtl::pair<PhysicalPage::mem_list *,size_t> right_sort(PhysicalPage::mem_list *&last_tail, size_t len_max) {
    if (last_tail == nullptr) return {nullptr, 0};
    if (len_max <= 4) {
        return branch_sort(last_tail, len_max);
    } else if (len_max <= 16) {
        return insert_sort(last_tail, len_max);
    } else {
        return merge_sort(last_tail, len_max);
    }
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