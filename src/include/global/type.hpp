#ifndef _GLOBAL_TYPE_HPP
#define _GLOBAL_TYPE_HPP
#pragma once
#ifdef _BITS_32
#ifdef _BITS_64
#error "danm define 32 and 64 bits for the same time!"
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
#elifdef _BITS_64
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