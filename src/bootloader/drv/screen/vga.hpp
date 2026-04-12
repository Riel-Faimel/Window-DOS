#pragma once
#ifndef __bootloader_drv_screen_vga_hpp
#define __bootloader_drv_screen_vga_hpp
#include "interface.hpp"

class VGA_text_mode : public _Screen {
    volatile unsigned short * buffer = (unsigned short *)0x000B8000;
    static constexpr unsigned int size = 0xBFFFF - 0xB8000;
    unsigned line = 0;
    unsigned offset = 0;
public:
    VGA_text_mode();
    ~VGA_text_mode() = default;

    void clear();
    void print(const char *str) volatile;

    bool do_it_has_graphic_mode();
    Screen_INFO get_info();
    void draw(
        unsigned left_down_x_offset, 
        unsigned left_down_y_offset,
        unsigned char *picture,
        unsigned picture_x_range,
        unsigned picture_y_range
    );
private:
    void _prt(char);
    void push() volatile;
    void full(unsigned char data, unsigned char attr = 0x00);
    
    void return_carriage() volatile;
    void newline() volatile;
    void back() volatile;
//debug
    void pt(const char *str, unsigned offset = 0);
};

class VGA_graphic_mode : public _Screen {};

#endif