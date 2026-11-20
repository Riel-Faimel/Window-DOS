#pragma once

namespace mem {

class MemSpace{
    // someone fill it 
};
struct MemAttr {
    // someone fill it
};

class Allocable {
public:
    virtual MemSpace alloc() = 0;
    virtual void dlloc(MemSpace) = 0;

    virtual void *alloc(MemSpace, size_t, MemAttr attr) = 0;
    virtual void dlloc(MemSpace, void *, size_t) = 0;
    
    virtual void *alloc(size_t) = 0;
    virtual void dlloc(void *, size_t = -1) = 0;

    virtual ~Allocable() = default;
};
    
} // namespace mem 