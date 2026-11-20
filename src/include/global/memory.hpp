#pragma once

#include "type.hpp"

namespace mem {

class PageNum {
    u64 data;
public:
    constexpr PageNum(): data{0u} {}
    constexpr explicit PageNum(u32 data_): data{data_} {}
    constexpr explicit PageNum(u64 data_): data{data_} {}
    constexpr explicit operator u64 () const { return data; }

    PageNum operator + (const PageNum &oth) const { return PageNum{data + oth.data}; }
    PageNum operator - (const PageNum &oth) const { return PageNum{data - oth.data}; }
    PageNum &operator += (const PageNum &oth) { data += oth.data; return *this; }
    PageNum &operator -= (const PageNum &oth) { data -= oth.data; return *this; }

    bool operator > (const PageNum &oth) const { return data > oth.data; }
    bool operator < (const PageNum &oth) const { return data < oth.data; }
    bool operator >= (const PageNum &oth) const { return data >= oth.data; }
    bool operator <= (const PageNum &oth) const { return data <= oth.data; }
};

class ByteNum {
    u64 data;
public:
    constexpr ByteNum(): data{0u} {}
    constexpr ByteNum(u32 h, u32 l): data{h, l} {}
    constexpr explicit ByteNum(u32 data_): data{data_} {}
    constexpr explicit ByteNum(u64 data_): data{data_} {}
    constexpr explicit ByteNum(PageNum pages): data{static_cast<u64>(pages) * 4096u} {}

    explicit operator u64 () const { return data; }
    explicit operator PageNum () { return PageNum{(data - 4095u) / 4096u}; }

    ByteNum operator + (const ByteNum &oth) const { return ByteNum{data + oth.data}; }
    ByteNum operator - (const ByteNum &oth) const { return ByteNum{data - oth.data}; }
    ByteNum &operator += (const ByteNum &oth) { data += oth.data; return *this; }
    ByteNum &operator -= (const ByteNum &oth) { data -= oth.data; return *this; }
};

class Address {
    u64 data;
public:
    Address() = delete;
    constexpr Address(u32 h, u32 l): data{h, l} {}
    constexpr explicit Address(void *data_): data{reinterpret_cast<u32>(data_)} {}
    constexpr explicit Address(u64 data_): data{data_} {}
    constexpr explicit Address(u32 data_): data{data_} {}
    Address &operator = (const Address &oth) = default;
    
    Address operator + (const ByteNum &bytesoffset) const 
    { return Address{data + static_cast<u64>(bytesoffset)}; }
    Address operator - (const ByteNum &bytesoffset) const 
    { return Address{data - static_cast<u64>(bytesoffset)}; }
    Address &operator += (const ByteNum &bytesoffset) {
        data += static_cast<u64>(bytesoffset);
        return *this;
    }
    Address &operator -= (const ByteNum &bytesoffset) {
        data -= static_cast<u64>(bytesoffset);
        return *this;
    }

    ByteNum operator - (const Address &oth) const { return ByteNum{data - oth.data}; }

    bool operator > (const Address &oth) const { return data > oth.data; }
    bool operator < (const Address &oth) const { return data < oth.data; }
    bool operator >= (const Address &oth) const { return data >= oth.data; }
    bool operator <= (const Address &oth) const { return data <= oth.data; }
    
    bool operator == (const Address &oth) const { return data == oth.data; }

    operator void *() const {
        if (data.high) return nullptr;
        else return (void *)data.low;
    }
};

template<typename T>
class Pointer {
    T *data;
public:
    Pointer() = delete;
    Pointer(Address addr) { data = (T *)(void *)(addr); }
    Pointer(T *data_): data{data_} {}
    operator T *() const { return data; }
    explicit operator u8 *() const { return static_cast<u8 *>(data); }
    explicit operator void *() const { return static_cast<void *>(data); }
    explicit operator Address () const { return Address{data}; }
    explicit operator u32 () const { return reinterpret_cast<u32>(data); }

    Pointer operator + (u32 i) const { return Pointer{static_cast<T *>(*this) + i}; }
    Pointer operator + (signed int i) const { return *this + (u32)i; }

    T &operator * () const { return *data; }
    T *operator -> () const { return data; }
};

template<>
class Pointer<u8> {
    u8 *data;
public:
    Pointer() = delete;
    Pointer(Address addr) { data = (u8 *)(void *)(addr); }
    Pointer(u8 *data_): data{data_} {}
    operator u8 *() const { return data; }
    explicit operator void *() const { return static_cast<void *>(data); }
    explicit operator Address () const { return Address{data}; }
    explicit operator u32 () const { return reinterpret_cast<u32>(data); }

    Pointer operator + (u32 i) const { return Pointer<u8>{data + i}; }

    u8 &operator * () const { return *data; }
    u8 *operator -> () const { return data; }
};

template<>
class Pointer<void> {
    void *data;
public:
    Pointer() = delete;
    Pointer(Address addr) { data = (void *)(addr); }
    Pointer(void *data_): data{data_} {}
    operator void *() const { return data; }
    explicit operator u8 *() const { return static_cast<u8 *>(data); }
    explicit operator Address () const { return Address{data}; }
    explicit operator u32 () const { return reinterpret_cast<u32>(data); }

    Pointer operator + (u32 i) const { return Pointer{static_cast<u8 *>(data) + i}; }

    void *operator -> () const { return data; }
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
    BoxLevel(u16 data_): data{data_} {}

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

} // namespace mem 