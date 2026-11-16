#ifndef __kernel_arch_x86_sche_x86_frame_hpp
#define __kernel_arch_x86_sche_x86_frame_hpp

#include <global/type.hpp>

/**
 * x86 register frame.
 *
 * This starts at X86TCB + offsetof(X86TCB, frame) == 24.  Keep the field
 * order and the NASM X86TCB layout in sync.
 */
#pragma pack(push, 1)
struct X86Frame {
    struct {
        size_t xdi = 0;
        size_t xsi = 0;
        size_t xbp = 0;
        size_t _rxsp = 0; // unused, kept for pusha layout
        size_t xbx = 0;
        size_t xdx = 0;
        size_t xcx = 0;
        size_t xax = 0;
    } context;

    size_t xip = 0;
    size_t xcs = 0;
    size_t xflag = 0b00000000000000000000001000000010;
    size_t xsp = 0;
    size_t xss = 0;

    size_t xfs = 0x33;
    size_t xds = 0;
    size_t xes = 0;
    size_t xgs = 0;
    size_t cr3 = 0;
    size_t ldtr = 0; // only 16 bits
};
#pragma pack(pop)

static_assert(sizeof(X86Frame) == 76);
static_assert(__builtin_offsetof(X86Frame, context) == 0);
static_assert(__builtin_offsetof(X86Frame, xip) == 32);
static_assert(__builtin_offsetof(X86Frame, xcs) == 36);
static_assert(__builtin_offsetof(X86Frame, xflag) == 40);
static_assert(__builtin_offsetof(X86Frame, xsp) == 44);
static_assert(__builtin_offsetof(X86Frame, xds) == 56);
static_assert(__builtin_offsetof(X86Frame, ldtr) == 72);

#endif
