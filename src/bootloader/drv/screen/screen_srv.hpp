#ifndef __bootloader_drv_screen_screen_srv_hpp
#define __bootloader_drv_screen_screen_srv_hpp

#include <global/type.hpp>
#include "vga.hpp"
void kprint(char *);
void kprint(const char *str);
void print_hex(unsigned char val, bool is_add_a_prefix = true);
void print_hex(unsigned short val, bool is_add_a_prefix = true);
void print_hex(unsigned val, bool is_add_a_prefix = true);
void print_char(char);

struct screen_output {
    inline screen_output &operator<< (char *s) { kprint(s);return *this; }
    inline screen_output &operator<< (const char *s) { kprint(s);return *this; }
    inline screen_output &operator<< (char c) { print_char(c);return *this; }
    inline screen_output &operator<< (unsigned char i) { print_hex(i);return *this; }
    inline screen_output &operator<< (unsigned short i) { print_hex(i);return *this; }
    inline screen_output &operator<< (unsigned i) { print_hex(i);return *this; }
    inline screen_output &operator<< (int i) { print_hex((unsigned)i);return *this; }
    inline screen_output &operator<< (u64 i) { print_hex(i.high);print_hex(i.low, false);return *this; }
    inline screen_output &operator<< (void *p) { print_hex((u32)p);return *this; }
};
extern screen_output cout;

#endif