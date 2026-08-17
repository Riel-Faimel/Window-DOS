#pragma once
#ifndef __bootloader_drv_screen_vga_hpp
#define __bootloader_drv_screen_vga_hpp

class VGA_text_mode {
    volatile unsigned short * buffer = (unsigned short *)0x000B8000;
    static constexpr unsigned int size = 0xBFFFF - 0xB8000;
    unsigned line = 0;
    unsigned offset = 0;
public:
    VGA_text_mode();
    ~VGA_text_mode() = default;

    void clear();
    void print(const char *str) volatile;
    void _prt(char);
private:
    void push() volatile;
    void full(unsigned char data, unsigned char attr = 0x00);
    
    void return_carriage() volatile;
    void newline() volatile;
    void back() volatile;
//debug
    void pt(const char *str, unsigned offset = 0);
};
#endif