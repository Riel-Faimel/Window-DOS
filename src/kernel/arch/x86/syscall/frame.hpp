#ifndef __kernel_arch_x86_syscall_frame_hpp
#define __kernel_arch_x86_syscall_frame_hpp

/**
 * Stack layout at syscall entry (low -> high):
 *
 *   [pusha] edi esi ebp esp ebx edx ecx eax
 *   [CPU iret frame] eip cs eflag (esp ss when coming from ring3)
 */
struct X86TrapFrame {
    unsigned edi;
    unsigned esi;
    unsigned ebp;
    unsigned esp;
    unsigned ebx;
    unsigned edx;
    unsigned ecx;
    unsigned eax;
    unsigned eip;
    unsigned cs;
    unsigned eflag;
};

static_assert(sizeof(X86TrapFrame) == 44, "X86TrapFrame must match pusha+iret");

#endif
