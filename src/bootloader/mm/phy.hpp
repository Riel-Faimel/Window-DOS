#ifndef __bootloader_mm_phy_hpp
#define __bootloader_mm_phy_hpp

#include <global/type.hpp>

#pragma pack(push, 1)
struct address_package {
    u64 address;
    u64 len;
};

struct PhysicalPage {
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

    class address_generator {
        size_t pageneedednums;
        bool need_continuous;

        friend class iterator;
    public:
        class iterator {
            address_generator* conditions;
            mem_list *thisptr;
            mem_list *lastptr;
            bool no_mem;
            bool done;
        public:
            iterator(address_generator*);

            address_package operator* ();
            bool operator!= (iterator&) { return !done; }
            iterator& operator++ () { return *this; }
        };

        address_generator(size_t, bool);
        ~address_generator();

        iterator begin() { return {this}; }
        iterator end() const { return {nullptr}; }
    };

    PhysicalPage();
    ~PhysicalPage();

    address_generator aloc(size_t page_nums, bool need_continuous = false);
    unsigned dloc(address_package mem);
    unsigned neaten();
};

extern PhysicalPage physicalpage;

#endif