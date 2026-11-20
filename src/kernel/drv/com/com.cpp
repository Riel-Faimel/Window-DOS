#include "com.hpp"

#include <port.h>

namespace {

// COM1 的端口基址：数据/中断/波特率/控制/状态。
constexpr unsigned short com1 = 0x3F8;
constexpr unsigned short reg_data = 0;  // DLAB=1 时是 divisor low
constexpr unsigned short reg_ier = 1;   // DLAB=1 时是 divisor high
constexpr unsigned short reg_fcr = 2;
constexpr unsigned short reg_lcr = 3;
constexpr unsigned short reg_mcr = 4;
constexpr unsigned short reg_lsr = 5;

constexpr unsigned char lsr_thr_empty = 0x20;

} // namespace

com_output comout;

com_output::com_output() {
    outb(0x00, com1 + reg_ier);        // 关中断
    outb(0x80, com1 + reg_lcr);        // DLAB，准备写分频
    outb(0x01, com1 + reg_data);       // 115200 baud
    outb(0x00, com1 + reg_ier);
    outb(0x03, com1 + reg_lcr);        // 8N1
    outb(0xC7, com1 + reg_fcr);        // 打开并清空 FIFO
    outb(0x0B, com1 + reg_mcr);        // DTR | RTS | OUT2
}

void com_output::write(const char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        while ((inb(com1 + reg_lsr) & lsr_thr_empty) == 0) {}
        outb(buf[i], com1 + reg_data);
    }
}
