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
    u16 time_size = 1;
    u16 remain_size = time_size;

//=== reg ===
    struct {
        /* stack top */
        size_t xdi = 0;
        size_t xsi = 0;
        size_t xbp = 0;
        size_t _rxsp = 0; // no to use
        size_t xbx = 0;
        size_t xdx = 0;
        size_t xcx = 0;
        size_t xax = 0;
        // stack bottum
    } context;

    size_t xip;
    size_t xcs;
    size_t xflag = 0b00000000000000000000001000000010;
    size_t xsp;
    size_t xss;

    size_t xfs = 0x33;
    size_t xds;
    size_t xes;
    size_t xgs;
    size_t cr3 = 0;
    size_t ldtr = 0; // only 16 bits

    TCB *prev = nullptr;
    size_t int_handler = 0;
    size_t wait_handler = 0;

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