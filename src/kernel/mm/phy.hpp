#ifndef __kernel_mm_phy_hpp
#define __kernel_mm_phy_hpp

#include <global/type.hpp>
#include "phy_def.hpp"

#pragma pack(push, 1)
struct PagePackage {
    mm::Address base;
    mm::PageNum pages;
};
#pragma pack(pop)

struct PhysicalPage {
    using PageNum = mm::PageNum;
    using Address = mm::Address;

    class address_generator {
        PageNum needpages;
        bool need_continuous;

        friend class iterator;
    public:
        class iterator {
            address_generator* conditions;
            struct mem_list *thisptr;
            struct mem_list *lastptr;
            bool no_mem;
            bool done;
        public:
            iterator(address_generator*);

            PagePackage operator* ();
            bool operator!= (iterator&) { return !done; }
            iterator& operator++ () { return *this; }
        };

        address_generator(PageNum, bool);
        ~address_generator();

        iterator begin() { return {this}; }
        iterator end() const { return {nullptr}; }
    };

    PhysicalPage();
    ~PhysicalPage();

    address_generator aloc(PageNum page_nums, bool need_continuous = false);
    unsigned dloc(PagePackage mem);
    unsigned neaten();
};

extern PhysicalPage physicalpage;

#endif