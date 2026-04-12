#ifndef __bootloader_CenterShell_cs_hpp
#define __bootloader_CenterShell_cs_hpp
#define _BITS_32
#include <global/type.hpp>

class CenterShell {
public:
    CenterShell();
private:
    void main_loop();
    void deal_keyboard_code();

    bool shift_on;
    bool control_on;
    bool win_on;
    bool alt_on;
    bool alt_gr_on;
    bool fn_on;
    
    bool caps_lock;
    bool num_lock;
    bool scroll_lock;
    bool insert_mode;

    bool extern_code;

    char line_buffer[512];
    unsigned line_offset;
};
#endif