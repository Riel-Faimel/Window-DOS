#include "context.hpp"

#include "tcb.hpp"

#include <global/new.hpp>
#include <mm/mm.hpp>

extern "C" {
    /**
     * Only used to make the initial exception frame valid.
     * The real scheduler hook is left to the H750 port.
     */
    [[noreturn]] void m32h750_thread_exit() {
        for (;;) asm volatile ("wfi\n");
    }

    TCB *current_tcb = nullptr;
    TCB *next_tcb = nullptr;
}

namespace arch {

M32H750Context::M32H750Context(MemoryManager &mm) : mm{mm} {
    // PendSV and SysTick are the lowest-priority exceptions.  PendSV must
    // never preempt a higher-priority exception handler.
    *(volatile u32 *)0xE000ED20 |= 0xFFFF0000u; // SHPR3: PendSV/SysTick = 0xFF
}

void M32H750TCB::tcb_push(TCBField field, size_t value) {
    // TODO: Cortex-M has no x86-style "push a word onto TCB" operation.
    // wait/cut need to rewrite the hardware exception frame instead.
    (void)field;
    (void)value;
}

void M32H750TCB::tcb_set_ip(size_t ip) {
    // sp -> r4..r11 (8 words = 32 bytes) -> r0..r3/r12/lr/pc/xpsr
    u32 *hw = (u32 *)(frame.sp + 32);
    hw[6] = (u32)ip | 1u; // PC, Thumb bit
}

TCB *M32H750Context::create_thread(
    void *func, size_t argc, void *argv, size_t stack_size
) {
    auto *tcb = new (mm) M32H750TCB{};
    auto *stack = new (mm) u8[stack_size];

    u32 top = ((u32)stack + (u32)stack_size) & ~7u; // AAPCS: 8-byte stack

    // Hardware exception frame, lowest address first:
    // r0 r1 r2 r3 r12 lr pc xpsr
    u32 *hw = (u32 *)top - 8;
    hw[0] = (u32)argc;                    // r0
    hw[1] = (u32)argv;                    // r1
    hw[2] = 0;                            // r2
    hw[3] = 0;                            // r3
    hw[4] = 0;                            // r12
    hw[5] = (u32)&m32h750_thread_exit | 1u; // lr, Thumb
    hw[6] = (u32)func | 1u;               // pc, Thumb
    hw[7] = 0x01000000u;                  // xpsr: T bit

    // Software frame below the hardware frame: r4..r11.
    u32 *sw = hw - 8;
    for (int i = 0; i < 8; i++) sw[i] = 0;

    tcb->frame.sp = (u32)sw;
    tcb->frame.stack_base = (u32)stack;
    tcb->frame.control = 0; // privileged thread mode, PSP

    return tcb;
}

[[noreturn]] void M32H750Context::switch_to(TCB &next) {
    next_tcb = &next;

    // Request PendSV.  The actual save/restore happens after the exception
    // return sequence, not in this C++ call.
    *(volatile u32 *)0xE000ED04 = (1u << 28); // ICSR.PENDSVSET
    asm volatile ("dsb\n" "isb\n" ::: "memory");

    // PendSV switches to the next thread; this call never resumes.
    for (;;) asm volatile ("wfi\n");
}

void M32H750Context::idle() {
    asm volatile ("wfi\n");
}

void M32H750Context::destroy(TCB *tcb) {
    auto *m32tcb = static_cast<M32H750TCB *>(tcb);
    operator delete ((void *)m32tcb->frame.stack_base, mm);
    operator delete (m32tcb, mm);
}

void M32H750Context::start_preemption(u32 reload) {
    // Lowest priority for PendSV/SysTick.
    *(volatile u32 *)0xE000ED20 |= 0xFFFF0000u;

    // SysTick: processor clock, reload value, clear current, enable + IRQ.
    *(volatile u32 *)0xE000E014 = reload;
    *(volatile u32 *)0xE000E018 = 0;
    *(volatile u32 *)0xE000E010 = 7; // CLKSOURCE | TICKINT | ENABLE
}

} // namespace arch

extern "C" void SysTick_Handler() {
    // TODO: this is the H750 port's scheduler tick hook.
    // It should call the generic DefaultScheduler::handle_trap(0) after
    // decrementing the current time slice.  Kept as a sketch so the
    // context-switch architecture is readable without a full CPU port.
    if (!current_tcb)
        return;

    if (current_tcb->remain_size)
        --current_tcb->remain_size;

    // The real port sets next_tcb through the scheduler before PendSV.
    // If the quantum expires, PendSV is requested from the scheduler path.
}
