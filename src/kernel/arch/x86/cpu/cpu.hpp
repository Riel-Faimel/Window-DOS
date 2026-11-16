#ifndef __kernel_multiCE_cpu_hpp
#define __kernel_multiCE_cpu_hpp

#include <arch/cpu.hpp>
#include <global/type.hpp>
#include <mm/mm.hpp>

#include "gdt.hpp"
#include "idt.hpp"
#include "tss.hpp"
#include "ldt.hpp"
#include <multiCE/sche/ds.hpp>
#include <arch/x86/sche/context.hpp>

#define CurruentArchitecture arch::x86

namespace arch {
    
struct x86 : public CPU {
    ::MemoryManager mm;// first
    IDT idt;
    GDT gdt;
    TSM tss; // table
    LDT ldt;
    X86Context context;
    alignas(4) DefaultScheduler scheduler; // schedule loop
    CPUInfo info;

    x86();

    CPUInfo get_info();

    // memory mode control
    void paging();
    void unpaging();

    // scheduler control
    void preempt() override;
    void collabora() override;
    void run(
        void (*)(size_t, void *), size_t, void *, size_t time = 1, u8 ring = 3,
        size_t cs = 0, size_t ds = 0, size_t gs = 0, size_t fs = 0
    ) override;

    // interrupt regist
    bool regist_int_handler(
        void (*handler)(void), unsigned internum, AccessControl access
    ) override;

    // segmention control
    const GDT &get_gdt() const { return gdt; }
};

} // namespace arch


#endif