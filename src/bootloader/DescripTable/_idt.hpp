#include "idt.hpp"

#include <hardlib/x86/port.h>
#include <drv/screen/screen_srv.hpp>
#include "intdis.hpp"
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
struct errcode_frame {
    unsigned errcode;
    unsigned eip;
    unsigned cs;
    unsigned eflags;
    unsigned esp;
    unsigned ss;
};
struct no_errcode_frame {
    unsigned eip;
    unsigned cs;
    unsigned eflags;
};
#pragma pack(pop)
void no_errcode_frame_status(no_errcode_frame *);
void errorcode_frame_status (errcode_frame *frame);

