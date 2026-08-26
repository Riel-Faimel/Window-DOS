#include "DOScall.hpp"

#include <global/type.hpp>
#include <drv/screen/screen_srv.hpp>
#include <DescripTable/idt.hpp>
#include <multiCE/default/cs.hpp>

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