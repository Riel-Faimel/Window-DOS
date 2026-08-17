#include "idt.hpp"

#include <hardlib/x86/port.h>
#include <drv/screen/screen_srv.hpp>
#include "IDNT.hpp"

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