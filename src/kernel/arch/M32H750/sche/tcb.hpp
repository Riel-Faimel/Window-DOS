#ifndef __kernel_arch_M32H750_sche_tcb_hpp
#define __kernel_arch_M32H750_sche_tcb_hpp

#include <interface/TD/TCB.hpp>

#include "frame.hpp"

namespace arch {

/**
 * Concrete Cortex-M7 TCB.
 *
 * TCB supplies the scheduling links.  M32H750Frame supplies the hardware
 * context anchor.  No allocator pointer is stored; ArchContext owns
 * destruction.
 */
struct M32H750TCB final : public TCB {
    M32H750Frame frame;

    M32H750TCB() = default;

    /**
     * Cortex-M does not have an x86-style stack frame.  The generic
     * wait/cut/wake paths need a Cortex-M-specific frame builder here.
     *
     * Left as a compile-time placeholder for the architecture sketch.
     */
    void tcb_push(TCBField field, size_t value = 0) override;

    /**
     * Patch the PC slot of the hardware exception frame.
     *
     * sp -> r4..r11 (32 bytes) -> r0..r3/r12/lr/pc/xpsr.
     */
    void tcb_set_ip(size_t ip) override;
};

static_assert(sizeof(M32H750TCB) == 36);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
static_assert(__builtin_offsetof(M32H750TCB, frame) == 24);
static_assert(__builtin_offsetof(M32H750Frame, sp) == 0);
#pragma GCC diagnostic pop

} // namespace arch

#endif
