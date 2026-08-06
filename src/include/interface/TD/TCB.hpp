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
    size_t xax = 0;
    size_t xbx = 0;
    size_t xcx = 0;
    size_t xdx = 0;

    size_t xsi = 0;
    size_t xdi = 0;

    size_t xss = 0;
    size_t xsp = 0;
    size_t xbp = 0;

    size_t xcs = 0;
    size_t xip = 0;

    size_t xds = 0;
    size_t xes = 0;

    size_t cr3 = 0;
    size_t xflag = 0;

#ifdef _BITS_64
    size_t xfs = 0;
    size_t xgs = 0;
    size_t r8 = 0;
    size_t r9 = 0;
    size_t r10 = 0;
    size_t r11 = 0;
    size_t r12 = 0;
    size_t r13 = 0;
    size_t r14 = 0;
    size_t r15 = 0;
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
    
    unsigned char time_size = 0;
};
#pragma pack(pop)

#endif