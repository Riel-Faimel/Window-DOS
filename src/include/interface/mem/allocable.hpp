#ifndef __include_interface_mem_allocable_hpp
#define __include_interface_mem_allocable_hpp

#include "krnlheapform.hpp"

namespace mem {

class MemSpace {
    u32 id = 0;
public:
    MemSpace() = default;
    explicit constexpr MemSpace(u32 pd) : id{pd} {}
    explicit constexpr operator u32() const { return id; }
    explicit constexpr operator bool() const { return id != 0; }
};

struct MemAttr {
    u32 want = 0; // image memory address which want to get
    u16 align = 0; // 0 = default 4KB
    u8 read : 1 = 0;
    u8 write : 1 = 0;
    u8 exec : 1 = 0;
};

class Allocable : public KernelHeapFormat {
protected:
    Allocable(SuperBlock *sb): KernelHeapFormat{sb} {}
public:
    virtual SuperBlock *getsbp() const { return sb; }

    virtual MemSpace alloc() = 0;
    virtual void dlloc(MemSpace) = 0;

    virtual void *alloc(MemSpace, size_t, MemAttr attr) = 0;
    virtual void dlloc(MemSpace, void *, size_t) = 0;
    
    virtual void *alloc(size_t) = 0;
    virtual void dlloc(void *, size_t = -1) = 0;

    virtual ~Allocable() = default;
};
    
} // namespace mem 

#endif