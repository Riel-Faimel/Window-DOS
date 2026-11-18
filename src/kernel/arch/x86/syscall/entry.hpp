#ifndef __kernel_arch_x86_syscall_entry_hpp
#define __kernel_arch_x86_syscall_entry_hpp

#include <interface/wm/syscall.hpp>
#include <interface/hardev/drvhead.hpp>
#include "frame.hpp"

/** Compile-time syscall-number policies (no arch-specific trap numbers). */
struct NativeEax {
    static size_t number(size_t eax) { return eax; }
};
struct DosAh {
    static size_t number(size_t eax) { return (eax >> 8) & 0xFF; }
};

/**
 * One parameterized entry generator per syscall.
 *
 * Handler : struct with   static void handle(SyscallArgs &);
 * Vector  : the trap vector this syscall chooses (not known to the asm body)
 * name    : a unique C symbol for this entry
 * Mode    : NativeEax / DosAh, how to read the syscall number from eax
 *
 * The naked asm body is identical for every syscall and never inspects eax,
 * the vector, the handler, or the syscall number.  Only name##_c knows them.
 */
#define SYSCALL_ENTRY(Handler, Vector, name, Mode)                          \
    extern "C" void name();                                                 \
    extern "C" void name##_c(X86TrapFrame *f) {                             \
        SyscallArgs a{};                                                    \
        a.number = Mode::number(f->eax);                                    \
        a.arg0 = f->ebx; a.arg1 = f->ecx;                                   \
        a.arg2 = f->edx; a.arg3 = f->esi;                                   \
        Handler::handle(a);                                                 \
        f->eax = a.ret;                                                     \
        if (a.error) f->eflag |= 1; else f->eflag &= ~1u;                   \
    }                                                                       \
    extern "C" __attribute__((naked)) void name() {                         \
        asm volatile (                                                      \
            "cli\n"                                                         \
            "pusha\n"                                                       \
            "push %%esp\n"                                                  \
            "call " #name "_c\n"                                            \
            "add $4, %%esp\n"                                               \
            "popa\n"                                                        \
            "sti\n"                                                         \
            "iret\n"                                                        \
            ::: "memory");                                                  \
    }                                                                       \
    struct name##_sh : SyscallHandler {                                     \
        void (*handler())() override { return &name; }                      \
        void handle(SyscallArgs &a) override { Handler::handle(a); }        \
    };                                                                      \
    InsideModuleRecord SyscallMod head {                                    \
        Vector,                                                             \
        []() -> SyscallHandler * { return new name##_sh; }                  \
    };

#endif
