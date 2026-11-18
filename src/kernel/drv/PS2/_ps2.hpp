#include "ps2.hpp"
#include <port.h>
#include <stdio>
#include <id/id.hpp>
#include <interface/hardev/drvhead.hpp>
#include <interface/knlobj/knlobj.hpp>
#include <KrnlObjMgr/kom.hpp>
#include "ps2_c.h"

#define DATA 0x60
#define CMD 0x64

class PS2_KB : public SerialPort {
    ps2 &drv;
    bool left_shift = false;
    bool right_shift = false;
    bool ctrl = false;
    bool alt = false;
    bool caps = false;
    bool extended = false;
    u8 skip = 0;
public:
    PS2_KB(ps2 &drv_): drv{drv_} {}

    unsigned take(void *, unsigned, unsigned) override;
    unsigned give(void *, unsigned, unsigned) override { return 0; }
};

class PS2_MS : public SerialPort {
    ps2 &drv;
public:
    PS2_MS(ps2 &drv_): drv{drv_} {}

    unsigned take(void *buf, unsigned, unsigned num) override {
        if (!buf) return 0;
        auto out = static_cast<u8 *>(buf);
        unsigned count = 0;
        u8 data;
        while (count < num && drv.mouse_buffer.pop(data))
            out[count++] = data;
        return count;
    }
    unsigned give(void *, unsigned, unsigned) override { return 0; }
};
