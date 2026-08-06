#ifndef __bootloader_drv_screen_screen_srv_hpp
#define __bootloader_drv_screen_screen_srv_hpp

#include "vga.hpp"
void kprint(char *);
void kprint(const char *str);
void print_hex(unsigned char val, bool is_add_a_prefix = true);
void print_hex(unsigned short val, bool is_add_a_prefix = true);
void print_hex(unsigned val, bool is_add_a_prefix = true);
void print_char(char);

#endif