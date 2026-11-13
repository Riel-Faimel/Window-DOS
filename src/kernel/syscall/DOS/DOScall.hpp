#ifndef __kernel_syscall_DOS_DOScall_hpp
#define __kernel_syscall_DOS_DOScall_hpp

#include <arch/x86/cpu/idt.hpp>

class DOScall {
    arch::IDT &idt;
public:
    DOScall(arch::IDT&);
    ~DOScall() = default;
};

#endif