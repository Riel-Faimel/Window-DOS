#ifndef __kernel_mm_phy_hpp
#define __kernel_mm_phy_hpp

#include <global/type.hpp>
#include <global/memory.hpp>

struct PhysicalPage {
    using PageNum = mem::PageNum;
    using Address = mem::Address;
    using PagePackage = mem::PagePackage;

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

    struct PageStatus {
        size_t total_mem;
        size_t free_mem;
    };
    PageStatus status();

private:
    size_t total_bytes = 0;
};

extern PhysicalPage physicalpage;

#endif