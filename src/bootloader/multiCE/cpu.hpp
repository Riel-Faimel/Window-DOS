#ifndef __bootloader_multiCE_cpu_hpp
#define __bootloader_multiCE_cpu_hpp

#include <mm/mm.hpp>
#include <DescripTable/gdt.hpp>
#include <DescripTable/idt.hpp>

class CPU {
    MemoryManager mm;// first
    IDT idt;
    GDT gdt;
public:
    CPU(): mm{}, idt{new descrptor::IDTEntry[256]}, gdt{new descrptor::Entry[256], 256, idt}{}
    CPU(volatile descrptor::IDTEntry*idtv, volatile descrptor::Entry*gdtv, int gdtc):
    mm{}, idt{idtv}, gdt{gdtv, gdtc, idt} {}
};

#endif