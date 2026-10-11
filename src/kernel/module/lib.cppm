module;

#include <global/type.hpp>

export module lib32;
extern "C" { extern void print_hex(unsigned short, bool = false); }

export extern "C" {
    __attribute__((noinline, regparm(2))) 
    void outb(char data, unsigned short port){
        asm volatile (
            "outb %0, %1"
            :
            : "a"(data), "d"(port)
            : "memory"
        );
    };
    __attribute__((noinline, regparm(2)))
    void outw(unsigned short data, unsigned short port){
        asm volatile (
            "outw %0, %1"
            :
            : "a"(data), "d"(port)
            : "memory"
        );
    };
    __attribute__((noinline, regparm(2))) 
    void outl(unsigned data, unsigned short port){
        asm volatile (
            "outl %0, %1"
            :
            : "a"(data), "d"(port)
            : "memory"
        );
    };
    __attribute__((noinline, regparm(1))) 
    unsigned char inb(unsigned short port){
        unsigned char data;
        asm volatile (
            "inb %1, %0"
            : "=a"(data)
            : "d"(port)
            : "memory"
        );
        return data;
    };
    __attribute__((noinline, regparm(1))) 
    unsigned short inw(unsigned short port){
        unsigned short data;
        asm volatile (
            "inw %1, %0"
            : "=a"(data)
            : "d"(port)
            : "memory"
        );
        return data;
    };
    __attribute__((noinline, regparm(1))) 
    unsigned inl(unsigned short port){
        unsigned data;
        asm volatile (
            "inl %1, %0"
            : "=a"(data)
            : "d"(port)
            : "memory"
        );
        return data;
    };
    void io_wait(){
        outb(0, 0x80);
    };
    void qps(const char *val){
        for(int i = 0;val[i];i++){
            outb(val[i], 0x3F8);
        }
    }; //QEMU print string
    
    __attribute__((used))
    void *memset(void *ptr, int value, size_t num) {
        unsigned char *p = (unsigned char *)ptr;
        unsigned char byte = (unsigned char)value; 

        for (size_t i = 0; i < num; i++) {
            p[i] = byte;
        }

        return ptr;
    }

    __attribute__((used))
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
        s = (const unsigned char *)sw;
        while (num--) {
            *d++ = *s++;
        }

        return dst;
    }
}