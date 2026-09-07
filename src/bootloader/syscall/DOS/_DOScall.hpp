#include "DOScall.hpp"

#include <global/type.hpp>
#include <drv/screen/screen_srv.hpp>
#include <arch/x86/idt.hpp>
#include <multiCE/default/cs.hpp>

#pragma pack(push, 1)
struct Frame {
    unsigned edi;
    unsigned esi;
    unsigned ebp;
    unsigned esp;
    unsigned ebx;
    unsigned edx;
    unsigned ecx;
    unsigned eax;
    unsigned eip;
    unsigned cs;
    unsigned eflag;
};
#pragma pack(pop)