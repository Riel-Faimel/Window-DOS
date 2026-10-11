#ifndef __kernel_mm_mm_def_hpp
#define __kernel_mm_mm_def_hpp

#include <global/type.hpp>
#include "phy_def.hpp"

namespace mm{

inline void *address_to_void_p(Address addr) {
    if (addr.data.high) return nullptr;
    else return (void *)addr.data.low;
}

template<typename T>
class Pointer {
    T *data;
public:
    Pointer() = delete;
    Pointer(Address addr) { data = (T *)address_to_void_p(addr); }
    Pointer(T *data_): data{data_} {}
    operator T *() const { return data; }
    explicit operator u8 *() const { return static_cast<u8 *>(data); }
    explicit operator void *() const { return static_cast<void *>(data); }
    explicit operator Address () const { return Address{data}; }
    explicit operator u32 () const { return reinterpret_cast<u32>(data); }

    T &operator * () const { return *data; }
    T *operator -> () const { return data; }
};

class BoxNum {
    size_t data;
public:
    BoxNum(size_t data_): data{data_} {}

    BoxNum operator + (const BoxNum &oth) const { return {data + oth.data}; }
    BoxNum &operator += (const BoxNum &oth) { data += oth.data; return *this; }
    BoxNum &operator ++ () { data++; return *this; }
    BoxNum operator ++ (int) { return {data++}; }
    BoxNum operator -- (int) { return {data--}; }
};

class BoxLevel {
    u16 data;
public:
    BoxLevel(): data{0} {}
    BoxLevel(u32 data_): data{data_} {}

    operator unsigned () const { return data; }
};

class SegmentSize {
    size_t pages;
public:
    SegmentSize(): pages{0} {}
    SegmentSize(size_t pages_): pages{pages_} {}
    SegmentSize(PageNum pages_): pages{static_cast<u64>(pages_).low} {}

    explicit operator PageNum () const { return PageNum{pages}; }
};
    
} // namespace mm


#endif