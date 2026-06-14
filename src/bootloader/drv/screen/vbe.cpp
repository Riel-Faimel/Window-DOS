#include "vbe.hpp"

inline VBE::VBE(VBE_info * /*info_base*/) {}

inline void VBE::clear() {
    unsigned int pixels = info.x_ * info.y_;
    unsigned int bytes_per_pixel = info.bpp / 8;
    
    if (bytes_per_pixel == 4) {
        unsigned int* fb = reinterpret_cast<unsigned int*>(info.base);
        for (unsigned int i = 0; i < pixels; i++) {
            fb[i] = 0;
        }
    }
    else if (bytes_per_pixel == 2) {
        unsigned short* fb = reinterpret_cast<unsigned short*>(info.base);
        for (unsigned int i = 0; i < pixels; i++) {
            fb[i] = 0;
        }
    }
    else {
        for (unsigned int i = 0; i < pixels * bytes_per_pixel; i++) {
            info.base[i] = 0;
        }
    }
}

inline void VBE::draw_gradient() {
    if(info.bpp == 32) {
        for(int y = 0; y < info.y_; y++) {
            for(int x = 0; x < info.x_; x++) {
                unsigned char r = (x * 255) / info.x_;
                unsigned char g = (y * 255) / info.y_;
                unsigned char b = 128;
                unsigned color = (0xFF << 24) | (r << 16) | (g << 8) | b;
                
                unsigned* pixel = (unsigned*)info.base + y * info.x_ + x;
                *pixel = color;
            }
        }
    }
    else if(info.bpp == 24) {
        for(int y = 0; y < info.y_; y++) {
            for(int x = 0; x < info.x_; x++) {
                unsigned char r = (x * 255) / info.x_;
                unsigned char g = (y * 255) / info.y_;
                unsigned char b = 128;
                
                unsigned char* pixel = info.base + (y * info.x_ + x) * 3;
                pixel[0] = b;
                pixel[1] = g;  
                pixel[2] = r;
            }
        }
    }
    else if(info.bpp == 16) {
        for(int y = 0; y < info.y_; y++) {
            for(int x = 0; x < info.x_; x++) {
                unsigned char r = (x * 31) / info.x_;
                unsigned char g = (y * 63) / info.y_;
                unsigned char b = 16;
                
                unsigned short color = (r << 11) | (g << 5) | b;
                
                unsigned short* pixel = (unsigned short*)info.base + y * info.x_ + x;
                *pixel = color;
            }
        }
    }
    else if(info.bpp == 8) {
        for(int y = 0; y < info.y_; y++) {
            for(int x = 0; x < info.x_; x++) {
                unsigned char index = (x + y) % 256;
                info.base[y * info.x_ + x] = index;
            }
        }
    }
}

inline void VBE::show_bpp_mode() {
    int bpp = info.bpp;
    int bytes = bpp / 8;
    
    for(int i = 0; i < info.x_ * info.y_; i++) {
        unsigned char* p = info.base + i * bytes;
        for(int b = 0; b < bytes; b++) p[b] = 0;
    }
    
    unsigned int color;
    if(bpp == 32) color = 0xFFFF0000;  // 红 = 32
    else if(bpp == 24) color = 0xFF00FF00; // 绿 = 24
    else if(bpp == 16) color = 0xFF0000FF; // 蓝 = 16
    else if(bpp == 8) color = 0xFFFFFF00;  // 黄 = 8
    else color = 0xFFFFFFFF; // 白 = 其他
    
    for(int i = 0; i < info.x_ * info.y_; i++) {
        unsigned char* p = info.base + i * bytes;
        if(bpp == 32) *(unsigned int*)p = color;
        else if(bpp == 24) {
            p[0] = color & 0xFF;
            p[1] = (color >> 8) & 0xFF;
            p[2] = (color >> 16) & 0xFF;
        }
        else if(bpp == 16) {
            unsigned short c = ((color>>16)&0xFF)>>3 <<11 | 
                            ((color>>8)&0xFF)>>2 <<5 | 
                            (color&0xFF)>>3;
            *(unsigned short*)p = c;
        }
        else if(bpp == 8) *p = color & 0xFF;
    }
}