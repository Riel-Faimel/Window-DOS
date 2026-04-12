#include "_cs.hpp"
u8 _char_buff[1024];

CenterShell::CenterShell():
shift_on(false), control_on(false), win_on(false),
alt_on(false), alt_gr_on(false), fn_on(false),
caps_lock(false), num_lock(false), scroll_lock(false), 
insert_mode(false),
line_offset(0), line_buffer{}{
    KeyBoard::init_driver();
    main_loop();
}

void CenterShell::main_loop(){
    screen->print("\rWelcome to WinDOS!\r\n");
    screen->print("mem:> ");
Back_label:
    asm volatile (
        "1: hlt\n"
        "jmp 1b\n"
    ); // wait for input and execute command
    asm volatile (
        ".globl Label_for_keyboard_interrput_goto\n"
        "Label_for_keyboard_interrput_goto:\n"
    );
    deal_keyboard_code();
    goto Back_label;
}

void CenterShell::deal_keyboard_code(){
    u8 code = KeyBoard::buffer[KeyBoard::read];
    KeyBoard::read = (KeyBoard::read + 1) % 1024;
    KeyBoard::click = false;
    //======
    if(extern_code){
        extern_code = false;
        
        // 扩展键处理
        switch (code) {
        case 0x1D:   // 右 Ctrl 按下
            control_on = true;
            return;
        case 0x9D:   // 右 Ctrl 释放
            control_on = false;
            return;
        case 0x38:   // 右 Alt (Alt Gr) 按下
            alt_gr_on = true;
            return;
        case 0xB8:   // 右 Alt 释放
            alt_gr_on = false;
            return;
        case 0x48:   // 上箭头
            // 处理方向键
            return;
        case 0x50:   // 下箭头
            return;
        case 0x4B:   // 左箭头
            return;
        case 0x4D:   // 右箭头
            return;
        case 0x52:   // Insert
            insert_mode = !insert_mode;
            return;
        case 0x53:   // Delete
            // 删除键
            return;
        case 0x47:   // Home
            return;
        case 0x4F:   // End
            return;
        case 0x49:   // Page Up
            return;
        case 0x51:   // Page Down
            return;
        }
        return;
    }
    
    switch (code){
    case 0xE0:
        extern_code = true;
        return;

    // Shift 键
    case 0x2A:   // 左 Shift 按下
    case 0x36:   // 右 Shift 按下
        shift_on = true;
        return;
    case 0xAA:   // 左 Shift 释放
    case 0xB6:   // 右 Shift 释放
        shift_on = false;
        return;
        
    // Ctrl 键
    case 0x1D:   // Ctrl 按下（左右相同）
        control_on = true;
        return;
    case 0x9D:   // Ctrl 释放
        control_on = false;
        return;
        
    // Alt 键
    case 0x38:   // Alt 按下（左右相同）
        alt_on = true;
        return;
    case 0xB8:   // Alt 释放
        alt_on = false;
        return;
        
    // Caps Lock
    case 0x3A:
        caps_lock = !caps_lock;
        return;
    // Num Lock
    case 0x45:
        num_lock = !num_lock;
        return;
    // Scroll Lock
    case 0x46:
        scroll_lock = !scroll_lock;
        return;
        
    default:
        break;
    };
    //normal code
    if(code & 0x80) return;
    char ch;
    if(shift_on)ch = shift_code2ascii[code];
    else ch = code2ascii[code];
    if(ch == 0)return;
    print_char(ch);
    line_buffer[line_offset] = ch;
    line_offset++;
    if(line_offset >= 512){
        line_offset = 0;
        return;
    }
    if(ch == '\b'){
        if(line_offset == 0)
        return;
        line_buffer[line_offset - 1] = '\0';
        line_offset-= 2;
    }
    if(ch == '\n'){
        cmd_shell(line_buffer, line_offset);
        line_offset = 0;
        for(unsigned i = 0;i < 512;i++)line_buffer[i] = 0;
        screen->print("mem:> ");
    }
}