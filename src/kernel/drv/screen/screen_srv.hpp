#ifndef __kernel_drv_screen_screen_srv_hpp
#define __kernel_drv_screen_screen_srv_hpp

#include <global/type.hpp>
#include <stdio>
#include "vga.hpp"
extern "C" { void kprint(char *); 
void print_hex(unsigned short val, bool is_add_a_prefix = true);}
void kprint(const char *str);
void kprint(char *str, char);
void print_hex(unsigned char val, bool is_add_a_prefix = true);
void print_hex(unsigned val, bool is_add_a_prefix = true);
void print_char(char);

class screen_output : public IOBackend {
    VGA_text_mode VGA_screen;
    friend void print_char(char);
    friend void kprint(char *);
    friend void kprint(char *, char);
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
    screen_output &operator<< (bool b) { if (b) kprint("true"); else kprint("false"); return *this; }
    void ch(char c) { print_char(c); }
    void str(const char *str, char end) { VGA_screen.print(str, end); }
    void hex(unsigned char i) { print_hex(i, false); }
    void hex(unsigned short i) { print_hex(i, false); }
    void hex(unsigned i) { print_hex(i, false); }
    void hex(int i) { print_hex((unsigned)i, false); }
};
extern screen_output vgatextout;

#endif