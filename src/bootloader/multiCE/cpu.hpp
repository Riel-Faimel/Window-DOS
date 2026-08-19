#ifndef __bootloader_multiCE_cpu_hpp
#define __bootloader_multiCE_cpu_hpp

#include <mm/mm.hpp>
#include <DescripTable/gdt.hpp>
#include <DescripTable/idt.hpp>
#include <global/type.hpp>
#include <TM/tss.hpp>
#include <multiCE/default/cs.hpp>
#include <TL/utility>

#pragma pack(push, 1)
struct CPU {
    size_t cpuid = 0;
    CPU *next = nullptr;
    MemoryManager mm;// first
    IDT idt;
    GDT gdt;
    TSM tss; // table
    DefaultScheduler scheduler; // schedule loop

    CPU();
    auto next_node(){ return next; }

private:
};
#pragma pack(pop)

extern CPU *cpu_list_root;

#endif