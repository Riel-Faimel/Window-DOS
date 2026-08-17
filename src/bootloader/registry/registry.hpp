#ifndef __bootloader_registry_registry_hpp
#define __bootloader_registry_registry_hpp
#pragma pack(push, 1)
struct _registry{
    unsigned char VGA_text_mode_screen_default_display_attribute;
    bool do_VGA_text_mode_always_use_carriage_return_after_newline;
    bool do_VGA_text_mode_always_use_full_space_after_back;
    bool do_IDE_controller_initialization_print_info;
};
#pragma pack(pop)
extern _registry registry;
class registry_editor{
public:
    registry_editor();
};
#endif