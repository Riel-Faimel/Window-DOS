#ifndef __bootloader_drv_screen_interface_hpp
#define __bootloader_drv_screen_interface_hpp
#pragma once

class _Screen{
public:
    struct Screen_INFO {};

    virtual void clear() = 0;
    virtual void print(const char *) volatile = 0;

    virtual bool do_it_has_graphic_mode() = 0;
    virtual Screen_INFO get_info() = 0;
    virtual void draw(
        unsigned left_down_x_offset, 
        unsigned left_down_y_offset,
        unsigned char *picture,
        unsigned picture_x_range,
        unsigned picture_y_range
    ) = 0;
private:
    virtual void _prt(char) = 0;
    friend void print_char(char);
};
#endif