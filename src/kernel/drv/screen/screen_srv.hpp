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

class screen_output : public OBackend {
    VGA_text_mode VGA_screen;
    friend void print_char(char);
    friend void kprint(char *);
    friend void kprint(char *, char);
public:
    void write(const char *buf, size_t len) override {
        for (size_t i = 0; i < len; i++) VGA_screen._prt(buf[i]);
    }
};
extern screen_output vgatextout;

#endif
