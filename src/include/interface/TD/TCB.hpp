#ifndef __include_TD_TCB_hpp
#define __include_TD_TCB_hpp

#include <global/type.hpp>

/**
 * for both rax/eax
 */
#pragma pack(push, 1)   
struct TCB {
//=== other ===
    TCB *next;
    size_t time_size = 1;

//=== reg ===
    struct {
        /* stack top */
        size_t xdi = 0;
        size_t xsi = 0;
        size_t xbp = 0;
        size_t xsp = 0;
        size_t xbx = 0;
        size_t xdx = 0;
        size_t xcx = 0;
        size_t xax = 0;
        // stack bottum
    } context;

    size_t xip = 0;
    size_t xcs = 0x08;
    size_t xflag = 0b00000000000000000000001000000010;
    size_t xss = 0x10;

    size_t xds = 0x10;
    size_t xes = 0x10;
    size_t xfs = 0x30;
    size_t xgs = 0x10;

    size_t cr3 = 0;

#ifdef _BITS_64
    size_t r8 = 0;
    size_t r9 = 0;
    size_t r10 = 0;
    size_t r11 = 0;
    size_t r12 = 0;
    size_t r13 = 0;
    size_t r14 = 0;
    size_t r15 = 0;
#endif

};
#pragma pack(pop)

#endif