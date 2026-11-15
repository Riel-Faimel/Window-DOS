#ifndef __kernel_drv_keyboard_keyboard_hpp
#define __kernel_drv_keyboard_keyboard_hpp

#include <global/type.hpp>
#include <id/id.hpp>
#include <HL/container>
#include <interface/hardev/drvhead.hpp>

class KeyBoard : public DeviceDriver {
    rtl::CyBack<char, 1024> buffer;
    bool click = false;
    static KeyBoard *self;
public:
    KeyBoard();
    static void keyboard_handler_c() __asm__ ("keyboard_handler_c");

    void work() override;
    void off() override;
};
#endif