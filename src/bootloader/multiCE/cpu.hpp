#ifndef __bootloader_multiCE_cpu_hpp
#define __bootloader_multiCE_cpu_hpp

#include <global/type.hpp>
#include <TL/utility>
#include <mm/mm.hpp>
#include <DescripTable/gdt.hpp>
#include <DescripTable/idt.hpp>
#include <DescripTable/tss.hpp>
#include <multiCE/default/cs.hpp>
#include <DOSsyscall/DOScall.hpp>

#pragma pack(push, 1)
struct CPU {
    size_t cpuid = 0;
    CPU *next = nullptr;
    MemoryManager mm;// first
    IDT idt;
    GDT gdt;
    TSM tss; // table
    DefaultScheduler scheduler; // schedule loop
    DOScall syscall;

    CPU();
    auto next_node(){ return next; }
    void preempt();
    void collabora();

    void run(
        void (*)(size_t, void *), size_t, void *, size_t time = 1, u8 ring = 3,
        size_t cs = 0, size_t ds = 0, size_t gs = 0, size_t fs = 0
    );

private:
};
#pragma pack(pop)

extern CPU *cpu_list_root;

#endif