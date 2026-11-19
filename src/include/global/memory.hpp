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
    friend void *address_to_void_p(Address);
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
};

} // namespace mem 