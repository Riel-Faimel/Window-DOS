#ifndef __kernel_arch_M32H750_sche_context_hpp
#define __kernel_arch_M32H750_sche_context_hpp

#include <global/type.hpp>
#include <interface/TD/context.hpp>

struct MemoryManager;
struct TCB;

namespace arch {

/**
 * Cortex-M7 context backend.
 *
 * switch_to() requests PendSV; the actual register save/restore happens in
 * context_asm.asm.  Threads run on PSP; interrupts run on MSP.
 */
class M32H750Context final : public ArchContext {
    MemoryManager &mm;

public:
    explicit M32H750Context(MemoryManager &mm);

    /**
     * Build a Cortex-M exception frame:
     *
     *   r0  = argc
     *   r1  = argv
     *   lr  = thread_exit
     *   pc  = func | 1
     *   xpsr= 0x01000000 (Thumb)
     *
     * plus an r4..r11 block below it.
     */
    TCB *create_thread(
        void *func, size_t argc, void *argv, size_t stack_size = 1024
    );

    [[noreturn]] void switch_to(TCB &next) override;
    void idle() override;
    void destroy(TCB *tcb) override;

    void start_preemption(u32 reload);
};

} // namespace arch

#endif
