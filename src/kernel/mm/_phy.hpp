#include "phy.hpp"

#include <arch/start.hpp>
#include <global/new.hpp>
#include <HL/generator>
#include <HL/utility>
#include <HL/atomic>
#include <stdio>
#include <global/macro.hpp>

#define StackSize 4096

#pragma pack(push, 1)
struct mmap_struc {
    u32 base_low;
    u32 base_high;
    u32 len_low;
    u32 len_high;
    enum Type : u32 {
        usable = 1,
        reserved = 2,
        ACPI = 3,
        NVS = 4,
        bad = 5,
    } type;
};

struct address_package {
    mem::Address address;
    mem::ByteNum len;
};

struct mem_list {
    address_package mem;
    mem_list *next;
    u8 exist;
    u8 r1;
    u8 r2;
    u8 r3;

    mem_list *next_node() { return next; }
};
#pragma pack(pop)
extern "C" {
    inline mmap_struc *mmap_struc_base = (mmap_struc *)0x5000;
    extern mem_list __kernel_end[];
}

struct __window_dos_loader_struct_0x1 {
    static mem_list* start;
    constexpr __window_dos_loader_struct_0x1() {}

    struct array {
        struct iterator {
            unsigned count = 1;
            bool done = false;
            inline mem_list *operator* () { return start+count; }
            constexpr bool operator!= (iterator&) { return !done; }
            iterator& operator++ ();
        };
        constexpr iterator begin() { return iterator{}; }
        constexpr iterator end() { return iterator{}; }
    };

    inline mem_list &operator[] (size_t offset) { return start[offset]; }
    inline mem_list *&operator *() { return start; }
    constexpr array as_arr() { return {}; }
    auto as_list() { return rtl::list_tranveser{start->next}; }
    constexpr mem_list *findspace();
} mem_list_root;


// =============

namespace {
    
rtl::pair<mem_list *, size_t> right_sort(mem_list *&last_tail, size_t len_max);
inline void append_or_merge(mem_list *&tail, mem_list *chosen, size_t& new_len) {
    if (tail->mem.address + tail->mem.len == chosen->mem.address) {
        tail->mem.len += chosen->mem.len;
        tail->next = chosen->next;
        new_len--;
    } else {
        tail->next = chosen;
        tail = tail->next;
    }
}
inline rtl::pair<mem_list*, size_t> merge(
    mem_list* llist, size_t llen,
    mem_list* rlist, size_t rlen
) {
    if (llist == nullptr) llen = 0;
    if (rlist == nullptr) rlen = 0;
    mem_list* result = nullptr;
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
        mem_list* chosen;
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
inline void cmp_swap(mem_list*& a, mem_list*& b) {
    if (a->mem.address > b->mem.address) {
        auto tmp = a; a = b; b = tmp;
    }
}
inline rtl::pair<mem_list*, size_t> branch_sort(
    mem_list *&last_tail, size_t len_max
) {
    if (last_tail == nullptr) return {nullptr, 0};
    
    mem_list* nodes[4];
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
inline rtl::pair<mem_list*, size_t> insert_sort(
    mem_list *&last_tail, size_t len_max
) {
    if (last_tail == nullptr) return {nullptr, 0};
    
    mem_list* nodes[16];
    size_t n = 0;
    while (last_tail != nullptr && n < len_max) {
        nodes[n++] = last_tail;
        last_tail = last_tail->next;
    }
    
    mem_list* sorted[16];
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
inline auto merge_sort(mem_list *&last_tail, size_t len_max) {
    auto left_len = len_max / 2, right_len = len_max-left_len;

    auto [llist, llen] = right_sort(last_tail, left_len);
    auto [rlist, rlen] = right_sort(last_tail, right_len);
    return merge(llist, llen, rlist, rlen);
}
/**
 * output: {result list, result lenght}
 * it push cursor by branch/insert sort
 */
rtl::pair<mem_list *,size_t> right_sort(mem_list *&last_tail, size_t len_max) {
    if (last_tail == nullptr) return {nullptr, 0};
    if (len_max <= 4) {
        return branch_sort(last_tail, len_max);
    } else if (len_max <= 16) {
        return insert_sort(last_tail, len_max);
    } else {
        return merge_sort(last_tail, len_max);
    }
}

} // namespace 