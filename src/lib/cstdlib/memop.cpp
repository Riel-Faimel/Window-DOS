#include <global/type.hpp>
#include <global/macro.hpp>
#pragma GCC optimize("O1")

extern "C" {
    
__attribute__((used, hot))
void *memset(void *ptr, int value, size_t num) {
    unsigned char *p = (unsigned char *)ptr;
    unsigned char byte = (unsigned char)value;

    for (size_t i = 0; i < num; i++) {
        p[i] = byte;
    }

    return ptr;
}

__attribute__((used, hot))
void *memcpy(void *dst, const void *src, size_t num) {
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;

    size_t *dw;
    const size_t *sw;

    while (num > 0 && ((size_t)d & (sizeof(size_t) - 1)) != 0) {
        *d++ = *s++;
        num--;
    }

    dw = (size_t *)d;
    sw = (const size_t *)s;
    while (num >= sizeof(size_t)) {
        *dw++ = *sw++;
        num -= sizeof(size_t);
    }

    d = (unsigned char *)dw;
    s = (unsigned char *)sw;
    while (num--) {
        *d++ = *s++;
    }

    return dst;
}

__attribute__((used, hot))
void *memmove(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    if (d == s || n == 0) return dest;

    if (d < s) {
        while (n && ((uintptr_t)d & (sizeof(size_t) - 1))) {
            *d++ = *s++; --n;
        }
        size_t *dw = (size_t *)d;
        const size_t *sw = (const size_t *)s;
        while (n >= sizeof(size_t)) {
            *dw++ = *sw++;
            n -= sizeof(size_t);
        }
        d = (unsigned char *)dw;
        s = (const unsigned char *)sw;
        while (n--) *d++ = *s++;
    } else {
        d += n; s += n;
        while (n && ((uintptr_t)d & (sizeof(size_t) - 1))) {
            *--d = *--s; --n;
        }
        size_t *dw = (size_t *)d;
        const size_t *sw = (const size_t *)s;
        while (n >= sizeof(size_t)) {
            *--dw = *--sw;
            n -= sizeof(size_t);
        }
        d = (unsigned char *)dw;
        s = (const unsigned char *)sw;
        while (n--) *--d = *--s;
    }
    return dest;
}

}