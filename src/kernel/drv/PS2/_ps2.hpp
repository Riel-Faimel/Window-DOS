#include "ps2.hpp"
#include <port.h>
#include <stdio>
#include <id/id.hpp>
#include <interface/hardev/drvhead.hpp>
#include <interface/knlobj/knlobj.hpp>
#include <interface/srvdev/stream.hpp>
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

/**
 * 键盘的流服务对象：只引用 PS2_KB，不把驱动细节暴露给使用者。
 *
 * 内部按块预取：方向键在驱动里是多字节 ESC 序列，逐字节 take 会让驱动的
 * "序列放不下"分支原地打转；这里一次取满一小块，再逐字节吐出去。
 */
class KBStream : public Stream {
    PS2_KB &kb;
    u8 buf[8];
    unsigned n = 0;
    unsigned pos = 0;
public:
    explicit KBStream(PS2_KB &kb_): kb{kb_} {}

    Stream &operator << (u8) override { return *this; }    // 键盘只读
    Stream &operator >> (u8 &b) override {
        if (pos >= n) {
            n = kb.take(buf, 0, sizeof buf);
            pos = 0;
            if (n == 0) { b = 0; return *this; }
        }
        b = buf[pos++];
        return *this;
    }

    void send() override {}
    void get() override {}
    void control() override {}
    void status() override {}
};

