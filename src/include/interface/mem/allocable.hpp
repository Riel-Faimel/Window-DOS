#ifndef __include_interface_mem_allocable_hpp
#define __include_interface_mem_allocable_hpp

#include "krnlheapform.hpp"
#include <global/memory.hpp>

namespace mem {

class Allocable : public KernelHeapFormat {
protected:
    Allocable(SuperBlock *sb): KernelHeapFormat{sb} {}
public:
    struct MemAttr {
        u32 want = 0; // image memory address which want to get
        u16 align = 0; // 0 = default 4KB
        u8 read : 1 = 0;
        u8 write : 1 = 0;
        u8 exec : 1 = 0;
    };
    virtual SuperBlock *getsbp() const { return sb; }

    // MemSpace create/delete
    virtual MemSpace alloc() = 0;
    virtual void dlloc(MemSpace) = 0;

    // alloc memory of MemSpace
    virtual void *alloc(MemSpace, size_t, MemAttr attr) = 0;
    virtual void dlloc(MemSpace, void *, size_t) = 0;
    
    // heap op
    virtual void *alloc(size_t) = 0;
    virtual void dlloc(void *, size_t = -1) = 0;

    virtual ~Allocable() = default;
};
    
} // namespace mem 

#endif