#ifndef __bootloader_drv_screen_screen_srv_hpp
#define __bootloader_drv_screen_screen_srv_hpp
void kprint(char *);
void print_hex(unsigned char val);
void print_hex(unsigned short val);
void print_hex(unsigned val);
void print_char(char);

#include <drv/screen/interface.hpp>

extern _Screen *screen;
#endif