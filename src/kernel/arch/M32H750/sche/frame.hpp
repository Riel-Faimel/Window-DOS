#ifndef __kernel_arch_M32H750_sche_frame_hpp
#define __kernel_arch_M32H750_sche_frame_hpp

#include <global/type.hpp>

/**
 * Cortex-M7 context anchor.
 *
 * Cortex-M hardware stacks r0-r3/r12/lr/pc/xpsr on exception entry.  The
 * software context switch (PendSV) saves r4-r11 below that hardware frame.
 *
 * sp points at the r4..r11 block.  The hardware frame starts at sp + 32.
 */
#pragma pack(push, 1)
struct M32H750Frame {
    u32 sp = 0;          // PSP -> r4..r11 block
    u32 stack_base = 0;  // allocation base for destroy
    u32 control = 0;     // CONTROL: thread mode / PSP / privilege
};
#pragma pack(pop)

static_assert(sizeof(M32H750Frame) == 12);
static_assert(__builtin_offsetof(M32H750Frame, sp) == 0);

#endif
