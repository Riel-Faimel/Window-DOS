#include "phy.hpp"

#include <arch/start.hpp>
#include <global/new.hpp>
#include <TL/generator>
#include <TL/utility>
#include <atomic>
#include <drv/screen/screen_srv.hpp>

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
    mm::Address address;
    mm::ByteNum len;
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
    extern mem_list* __boot_loader_end;
}

struct __window_dos_loader_struct_0x1 {
    using mem_list = mem_list;
    static mem_list* start;
    constexpr __window_dos_loader_struct_0x1() {}

    struct array {
        struct Iterator {
            unsigned count = 1;
            bool done = false;
            inline mem_list *operator* () { return start+count; }
            constexpr bool operator!= (Iterator&) { return !done; }
            Iterator& operator++ ();
        };
        constexpr Iterator begin() { return Iterator{}; }
        constexpr Iterator end() { return Iterator{}; }
    };

    inline mem_list &operator[] (size_t offset) { return start[offset]; }
    inline mem_list *&operator *() { return start; }
    constexpr array as_arr() { return {}; }
    auto as_list() { return rtl::list_tranveser{start->next}; }
    constexpr mem_list *findspace();
} mem_list_root;