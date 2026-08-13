#ifndef _GLOBAL_TYPE_HPP
#define _GLOBAL_TYPE_HPP

#ifdef _BITS_32
#ifdef _BITS_64
#error "defined 32 and 64 bits for the same time!"
#endif
#endif

#define NULL_PTR 0

using s8 = signed char;
using u8 = unsigned char;
using __l8 = u8;

using s16 = signed short;
using u16 = unsigned short;
using __l16 = u16;

using s32 = signed int;
using u32 = unsigned int;
using __l32 = u32;

//only base on Windows_64
#ifdef _BITS_32
using size_t = unsigned int;
using uintptr_t = unsigned long;
using intptr_t = long;

struct u64 {
    u32 high;
    u32 low;

    constexpr u64() : high(0), low(0) {}
    constexpr u64(u32 val) : high(0), low(val) {}
    constexpr u64(u32 h, u32 l) : high(h), low(l) {}

    constexpr operator u32 () { return low; }

    constexpr u64 operator+(const u64& other) const {
        u64 result;
        result.low = low + other.low;
        result.high = high + other.high + (result.low < low ? 1 : 0);
        return result;
    }
    constexpr u64 operator+(const u32& other) const {
        return u64{high, low+other};
    }
    constexpr u64 operator-(const u64& other) const {
        u64 result;
        result.low = low - other.low;
        result.high = high - other.high - (low < other.low ? 1 : 0);
        return result;
    }
    constexpr u64 operator-(const u32& other) {
        return u64{high, low-other};
    }
    template<typename T>
    constexpr u64 &operator+= (const T& other) {
        *this = *this + other;
        return *this;
    }
    template<typename T>
    constexpr u64 &operator-= (const T& other) {
        *this = *this - other;
        return *this;
    }
    
    constexpr bool operator==(const u64& other) const {
        return high == other.high && low == other.low;
    }
    constexpr bool operator!=(const u64& other) const {
        return high != other.high || low != other.low;
    }
    constexpr bool operator<(const u64& other) const {
        if (high != other.high) {
            return high < other.high;
        }
        return low < other.low;
    }
    constexpr bool operator>(const u64& other) const {
        if (high != other.high) {
            return high > other.high;
        }
        return low > other.low;
    }
    constexpr bool operator<=(const u64& other) const {
        if (high != other.high) {
            return high < other.high;
        }
        return low <= other.low;
    }
    constexpr bool operator>=(const u64& other) const {
        if (high != other.high) {
            return high > other.high;
        }
        return low >= other.low;
    }

    constexpr bool operator==(const u32& other) const {
        return high == 0 && low == other;
    }
    constexpr bool operator!=(const u32& other) const {
        return high != 0 || low != other;
    }
    constexpr bool operator<(const u32& other) const {
        if (high != 0) { return false; }
        return low < other;
    }
    constexpr bool operator>(const u32& other) const {
        if (high != 0) { return high > 0; }
        return low > other;
    }
    constexpr bool operator<=(const u32& other) const {
        if (high != 0) { return false; }
        return low <= other;
    }
    constexpr bool operator>=(const u32& other) const {
        if (high != 0) { return high > 0; }
        return low >= other;
    }

    constexpr u64 operator|(const u64& other) const { return u64(high | other.high, low | other.low); }
    constexpr u64 operator&(const u64& other) const { return u64(high & other.high, low & other.low); }

    constexpr u64 operator<<(int bits) const {
        if (bits <= 0) return *this;
        if (bits >= 64) return u64(0, 0);
        if (bits >= 32) return u64(low << (bits - 32), 0);
        return u64((high << bits) | (low >> (32 - bits)), low << bits);
    }
    constexpr u64 operator>>(int bits) const {
        if (bits <= 0) return *this;
        if (bits >= 64) return u64(0, 0);
        if (bits >= 32) return u64(0, high >> (bits - 32));
        return u64(high >> bits, (low >> bits) | (high << (32 - bits)));
    }

    constexpr u64 operator*(const u64& other) = delete;
    constexpr u64 operator*(u32 other) const {
        u64 result(0, 0);
        u64 tmp = *this;
        while (other) {
            if (other & 1) result += tmp;
            tmp = tmp << 1;
            other >>= 1;
        }
        return result;
    }
    constexpr u64 operator/(const u64& other) = delete;
    constexpr u64 operator/(u32 divisor) const {
        if (divisor == 0) return u64(0, 0);

        u64 quotient(0, 0);
        u64 remainder(0, 0);

        for (int i = 63; i >= 0; i--) {
            remainder = (remainder << 1) | u64(0, (*this >> i).low & 1);
            if (remainder >= u64(0, divisor)) {
                remainder = remainder - u64(0, divisor);
                quotient = quotient | (u64(0, 1) << i);
            }
        }
        return quotient;
    }
    constexpr u64 operator%(const u64& other) = delete;
    constexpr u64 operator%(u32 divisor) const {
        if (divisor == 0) return u64(0, 0);

        u64 remainder(0, 0);
        for (int i = 63; i >= 0; i--) {
            remainder = (remainder << 1) | u64(0, (*this >> i).low & 1);
            if (remainder >= u64(0, divisor)) {
                remainder = remainder - u64(0, divisor);
            }
        }
        return remainder;
    }
};

#endif
#ifdef _BITS_64
using size_t = unsigned long long;
using uintptr_t = unsigned long long;
using intptr_t = long long;

using s64 = signed long long;
using u64 = unsigned long long;
using __l64 = u64;
#endif

template<typename __l>
class le;

template<typename __l>
class be;

#pragma pack(push, 1)
template<typename __l>
class le {
    __l storage;

public:
    le():storage(0){}
    le(__l l): storage(l){}
    le(be<__l> b): storage(0) {
        for(int i = 0;i < sizeof(__l);i++) {
            storage |= ((b >> (8 * i)) & 0xFF) << ((sizeof(__l) - i - 1) * 8);
        }
    }
    ~le() = default;

    operator __l() const {
        return storage;
    }

    operator be<__l>() const {
        be<__l> re;
        for(int i = 0;i < sizeof(__l);i++) {
            re.storage |= ((storage >> (8 * i)) & 0xFF) << ((sizeof(__l) - i - 1) * 8);
        }
        return re;
    }

    le &operator= (__l l) {
        storage = l;
        return *this;
    }

    le &operator= (const be<__l> &b) {
        storage = 0;
        for(int i = 0;i < sizeof(__l);i++){
            storage |= ((b >> (8 * i)) & 0xFF) << ((sizeof(__l) - i - 1) * 8);
        };
        return *this;
    }

    friend class be<__l>;
};

template<typename __l>
class be {
    __l storage;

public:
    be(): storage(0){}
    be(__l l): storage(0) {
        for(int i = 0;i < sizeof(__l);i++){
            storage |= ((l >> (8 * i)) & 0xFF) << ((sizeof(__l) - i - 1) * 8);
        };
    }
    be(le<__l> l): storage(0) {
        for(int i = 0;i < sizeof(__l);i++){
            storage |= ((l >> (8 * i)) & 0xFF) << ((sizeof(__l) - i - 1) * 8);
        };
    }
    ~be() = default;

    operator __l() const {
        __l re;
        for(int i = 0;i < sizeof(__l);i++){
            re |= ((storage >> (8 * i)) & 0xFF) << ((sizeof(__l) - i - 1) * 8);
        }
        return re;
    }

    operator le<__l> () const {
        le<__l> re;
        for(int i = 0;i < sizeof(__l);i++){
            re.storage |= ((storage >> (8 * i)) & 0xFF) << ((sizeof(__l) - i - 1) * 8);
        }
        return re;
    }

    be &operator= (__l l){
        storage = 0;
        for(int i = 0;i < sizeof(__l);i++){
            storage |= ((l >> (8 * i)) & 0xFF) << ((sizeof(__l) - i - 1) * 8);
        }
        return *this;
    }

    be &operator= (const le<__l> l){
        storage = 0;
        for(int i = 0;i < sizeof(__l);i++){
            storage |= ((l >> (8 * i)) & 0xFF) << ((sizeof(__l) - i - 1) * 8);
        }
        return *this;
    }

    friend class le<__l>;
};
#pragma pack(pop)

using BYTE = __l8;
using WORD = __l16;
using STXT = const char *;

#endif