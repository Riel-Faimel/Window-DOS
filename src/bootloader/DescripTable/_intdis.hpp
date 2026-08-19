#include "intdis.hpp"

#include "idt.hpp"
#include <multiCE/cpu.hpp>
#include <TL/generator>
#include <drv/screen/screen_srv.hpp>

#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1