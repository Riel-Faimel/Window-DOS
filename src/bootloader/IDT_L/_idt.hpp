#include "idt.hpp"

#include <cstdlib/port.h>
#include <drv/screen/screen_srv.hpp>
#include "IDNT.hpp"

#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1

extern "C" void DE_handler();
extern "C" void OF_handler();
extern "C" void UD_handler();
extern "C" void NM_handler();
extern "C" void DF_handler();
extern "C" void GP_handler();
extern "C" void basic_time_handler();

extern "C" void catch_program();

#pragma pack(push, 1)
struct _program_status {
    unsigned cs;
    unsigned eip;
    unsigned eflags;
    //unsigned esp;
    //unsigned ss;
    //unsigned ebp;
    //unsigned error_code;
};
#pragma pack(pop)
void print_program_status(_program_status *);