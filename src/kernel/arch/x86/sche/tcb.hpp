#ifndef __kernel_arch_x86_sche_x86_tcb_hpp
#define __kernel_arch_x86_sche_x86_tcb_hpp

#include <interface/TD/TCB.hpp>

#include "frame.hpp"

namespace arch {

/**
 * Concrete x86 TCB.
 *
 * The generic scheduling links live in TCB; the architectural frame lives
 * here.  No allocator pointer is stored: ArchContext owns destruction.
 */
struct X86TCB final : public TCB {
    X86Frame frame;

    X86TCB() = default;

    void tcb_push(TCBField field, size_t value = 0) override;
    void tcb_set_ip(size_t ip) override;
};

static_assert(sizeof(X86TCB) == 100);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
static_assert(__builtin_offsetof(X86TCB, frame) == 24);
#pragma GCC diagnostic pop

} // namespace arch

#endif
