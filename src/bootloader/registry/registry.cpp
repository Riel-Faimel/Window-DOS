#include "registry.hpp"
_registry registry;

registry_editor::registry_editor(){
    registry.do_VGA_text_mode_always_use_carriage_return_after_newline = true;
    registry.do_VGA_text_mode_always_use_full_space_after_back = true;
    registry.VGA_text_mode_screen_default_display_attribute = 0x1F;
    registry.do_IDE_controller_initialization_print_info = false;
}