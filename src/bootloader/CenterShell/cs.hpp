#ifndef __bootloader_CenterShell_cs_hpp
#define __bootloader_CenterShell_cs_hpp
#define _BITS_32
#include <global/type.hpp>
#include <lib/cppstdlib/string>
#define __independent_lib_Using_template_container
#include <TL/idlib>

class CenterShell {
public:
    struct Command_list {
        String cmd_name;
        void (*handler)(String);
    };
    CenterShell();

    void register_other_shell(void (*handler)(char));
    void unregister_other_shell();

    void reg_cmd(String, void(*)(String));
    void unreg_cmd(String);
private:
    void main_loop();
    void deal_keyboard_code();

    bool if_is_other_shell;
    void (*handler)(char);

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
    //=======
    void extern_shell(char *, unsigned);
    rtl::array<Command_list> CMD_List{4};
    String cmd_prompt = "mem:";
};
#endif