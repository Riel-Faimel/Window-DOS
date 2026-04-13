#include "_cs.hpp"
u8 _char_buff[1024];

CenterShell::CenterShell():
shift_on(false), control_on(false), win_on(false),
alt_on(false), alt_gr_on(false), fn_on(false),
caps_lock(false), num_lock(false), scroll_lock(false), 
insert_mode(false), extern_code(false),
line_offset(0), line_buffer{},
if_is_other_shell(false), handler(nullptr){
    KeyBoard::init_driver();
    main_loop();
}

void CenterShell::main_loop(){
    linear_address_space->reg_cmd(this);
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
    if(if_is_other_shell){
        handler(code);
        return;
    } //send to other shell if exist
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
        extern_shell(line_buffer, line_offset);
        line_offset = 0;
        for(unsigned i = 0;i < 512;i++)line_buffer[i] = 0;
        screen->print(cmd_prompt);screen->print("> ");
    }
}

void CenterShell::register_other_shell(void (*handler_)(char)){
    handler = handler_;
    if_is_other_shell = true;
}

void CenterShell::unregister_other_shell(){
    handler = nullptr;
    if_is_other_shell = false;
}

void CenterShell::extern_shell(char *line_buffer, unsigned line_size){
    String cmd_line(line_buffer, line_size);
    cmd_line = cmd_line.trim();

    if(cmd_line[cmd_line.length() - 1] == ':' && cmd_line.length() >= 2 && cmd_line.length() <= 7){
        if(linear_address_space->choose_disk(cmd_line)){
            cmd_prompt = cmd_line;
        } //disk
        return;
    }

    for(unsigned i = 0;i < CMD_List.get_size();i++){
        if(CMD_List[i].cmd_name == cmd_line.substr(0, CMD_List[i].cmd_name.length())){
            CMD_List[i].handler(cmd_line);
            return;
        }
    }

    cmd_shell(line_buffer, line_size);
}

void CenterShell::reg_cmd(String cmd_name, void (*func)(String)){
    CMD_List.append({cmd_name, func});
}

void CenterShell::unreg_cmd(String str){
    for(unsigned i = 0;i < CMD_List.get_size();i++){
        if(CMD_List[i].cmd_name == str){
            CMD_List[i] = {};
            return;
        }
    }
}
