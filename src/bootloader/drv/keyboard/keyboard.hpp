#ifndef __bootloader_drv_keyboard_keyboard_hpp
#define __bootloader_drv_keyboard_keyboard_hpp

#include <global/type.hpp>
#include <DescrpTable/idt_.hpp>
namespace KeyBoard {
extern char buffer[1024];
extern bool click;
extern unsigned read;
void keyboard_handler_c() __asm__ ("keyboard_handler_c");
void init_driver();
}
#endif