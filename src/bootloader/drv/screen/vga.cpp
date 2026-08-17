#include "vga.hpp"
#include <registry/registry.hpp>

import lib32;

// === VGA text mode ===
VGA_text_mode VGA_screen;

#define LINE_SIZE 80
#define LINE_NUM 25

VGA_text_mode::VGA_text_mode(){
    clear();
    screen = this;
};

void VGA_text_mode::_prt(char ch){
    switch(ch){
    case '\r':
        return_carriage();
        break;
    case '\n':
        newline();  // 换行
        return;
    case '\b':
        back();
        return;
    default:
        buffer[line * LINE_SIZE + offset] = registry.VGA_text_mode_screen_default_display_attribute << 8 | ch;
        offset++;
        break;
    }
    if(offset >= LINE_SIZE){  // 行满自动换行
        newline();
        return_carriage();
    }
}

void VGA_text_mode::full(unsigned char data, unsigned char attr){
    unsigned char style;
    if(attr)style = attr;
    else style = registry.VGA_text_mode_screen_default_display_attribute;
    for (unsigned i = 0;i < LINE_SIZE * LINE_NUM;i++){
        buffer[i] = style << 8 | data;
    }
}

void VGA_text_mode::clear(){
    full(' ');
    line = 0;
    offset = 0;
}

void VGA_text_mode::push() volatile{
    unsigned i = 0;
    for(;i < LINE_NUM - 1;i++){
        for(unsigned j = 0;j < LINE_SIZE;j++){
            buffer[LINE_SIZE * i + j] = buffer[LINE_SIZE * (i + 1) + j];
        }
    }
    for(unsigned j = 0;j < LINE_SIZE;j++){
        buffer[LINE_SIZE * i + j] = (registry.VGA_text_mode_screen_default_display_attribute << 8) | ' ';
    }
}

void VGA_text_mode::print(const char *str) volatile {
    for(unsigned i = 0; str[i]; i++){
        switch(str[i]){
        case '\r':
            return_carriage();
            break;
        case '\n':
            newline();  // 换行
            continue;
        case '\b':
            back();
            continue;
        default:
            buffer[line * LINE_SIZE + offset] = registry.VGA_text_mode_screen_default_display_attribute << 8 | str[i];
            offset++;
            break;
        }
        if(offset >= LINE_SIZE){  // 行满自动换行
            newline();
            return_carriage();
        }
    };
}

void VGA_text_mode::return_carriage() volatile {
    offset = 0;
}

void VGA_text_mode::newline() volatile {
    line++;
    if(line < LINE_NUM){
        if(registry.do_VGA_text_mode_always_use_carriage_return_after_newline)offset = 0;
        return ;
    }
    push();
    line = LINE_NUM - 1;
    if(registry.do_VGA_text_mode_always_use_carriage_return_after_newline)offset = 0;
}

void VGA_text_mode::back() volatile{
    if (offset > 0) {
        offset--;
        if(registry.do_VGA_text_mode_always_use_full_space_after_back)buffer[LINE_SIZE * line + offset] = (registry.VGA_text_mode_screen_default_display_attribute << 8) | ' ';
    } else if (line > 0) {
        line--;
        offset = LINE_SIZE - 1;
        if(registry.do_VGA_text_mode_always_use_full_space_after_back)buffer[LINE_SIZE * line + offset] = (registry.VGA_text_mode_screen_default_display_attribute << 8) | ' ';
    }
}

void VGA_text_mode::pt(const char *str, unsigned offset){
    for(unsigned i = 0;str[i];i++){
        buffer[offset + i] = 0x1F00 | str[i];
    }
}