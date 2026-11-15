#ifndef __kernel_drv_ps2_ps2_hpp
#define __kernel_drv_ps2_ps2_hpp

#include <global/type.hpp>
#include <id/id.hpp>
#include <HL/container>
#include <interface/hardev/drvhead.hpp>

class ps2 : public DeviceDriver {
    static ps2 *self;
public:
    rtl::CyBack<u8, 1024> buffer;
    rtl::CyBack<u8, 1024> mouse_buffer;
    ps2();
    static void ps2_handler_c() __asm__ ("ps2_handler_c");

    void work() override;
    void off() override;
};
#endif
