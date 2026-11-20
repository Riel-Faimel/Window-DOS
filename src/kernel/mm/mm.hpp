#ifndef __kernel_mm_mm_hpp
#define __kernel_mm_mm_hpp

#include "phy.hpp"
#include <global/new.hpp>
#include <interface/mem/allocable.hpp>
#include <interface/mem/krnlheapform.hpp>

class MemoryManager {
        /**
         * kernel heap allocator
         * when no paging, we use physical memory know from in paging
         * 
         * all of them offered alloc/dlloc function
         * and called by operator new/delete
         */
    struct nopage : public mem::Allocable, public mem::KernelHeapFormat {
        nopage(void* page_base);
        
        mem::MemSpace alloc() override;
        void dlloc(mem::MemSpace) override;

        void *alloc(mem::MemSpace, size_t, mem::MemAttr) override;
        void dlloc(mem::MemSpace, void *, size_t) override;

        void *alloc(size_t) override;
        void dlloc(void *, size_t) override;
    };

    nopage defaul;
    mem::Allocable *al;


public:
    MemoryManager(bool = true);
    MemoryManager(mem::Allocable *);

    void set_allocator(mem::Allocable *);
    mem::Allocable *allocator() const { return al; }
};

#endif