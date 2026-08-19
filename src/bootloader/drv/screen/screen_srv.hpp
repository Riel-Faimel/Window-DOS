#ifndef __bootloader_drv_screen_screen_srv_hpp
#define __bootloader_drv_screen_screen_srv_hpp

#include <global/type.hpp>
#include "vga.hpp"
extern "C" { void kprint(char *); 
void print_hex(unsigned short val, bool is_add_a_prefix = true);}
void kprint(const char *str);
void print_hex(unsigned char val, bool is_add_a_prefix = true);
void print_hex(unsigned val, bool is_add_a_prefix = true);
void print_char(char);

class screen_output {
    VGA_text_mode VGA_screen;
    friend void print_char(char);
    friend void kprint(char *);
public:
    screen_output &operator<< (char *s) { kprint(s);return *this; }
    screen_output &operator<< (const char *s) { kprint(s);return *this; }
    screen_output &operator<< (char c) { print_char(c);return *this; }
    screen_output &operator<< (unsigned char i) { print_hex(i);return *this; }
    screen_output &operator<< (unsigned short i) { print_hex(i);return *this; }
    screen_output &operator<< (unsigned i) { print_hex(i);return *this; }
    screen_output &operator<< (int i) { print_hex((unsigned)i);return *this; }
    screen_output &operator<< (unsigned long long i) { print_hex((unsigned)i);return *this; }
    screen_output &operator<< (u64 i) { print_hex(i.high);print_hex(i.low, false);return *this; }
    screen_output &operator<< (void *p) { print_hex((u32)p);return *this; }
};
extern screen_output cout;

#endif