#ifndef __include_TD_TCB_hpp
#define __include_TD_TCB_hpp

#include <global/type.hpp>

/**
 * for both rax/eax
 */
#pragma pack(push, 1)   
struct TCB {
    size_t thread_ID;

//=== reg ===
    size_t xax;
    size_t xbx;
    size_t xcx;
    size_t xdx;

    size_t xsi;
    size_t xdi;

    size_t xss;
    size_t xsp;
    size_t xbp;

    size_t xcs;
    size_t xip;

    size_t xds;
    size_t xes;

    size_t cr3;
    size_t xflag;

#ifdef _BITS_64
    size_t xfs;
    size_t xgs;
    size_t r8;
    size_t r9;
    size_t r10;
    size_t r11;
    size_t r12;
    size_t r13;
    size_t r14;
    size_t r15;
#endif

//=== other ===

    /**
     * change only by multiCE
     * scheduler can read only
     */
    enum class State : unsigned char {
        Die,
        Run,
        Wait,
    };
    State state;
    
    unsigned char time_size;
};
#pragma pack(pop)

#endif