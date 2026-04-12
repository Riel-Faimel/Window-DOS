#ifndef __bootloader_drv_screen_vbe_hpp
#define __bootloader_drv_screen_vbe_hpp

#include "interface.hpp"

#pragma pack(push, 1)
struct VBE_info{
    unsigned char *base;
    short x_;
    short y_;
    char bpp;
};
#pragma pack(pop)

class VBE : public _Screen {
public:
    VBE_info info;

public:
    VBE(VBE_info *info_base);

    void clear();

    void draw_gradient();

    void show_bpp_mode();
};
#endif