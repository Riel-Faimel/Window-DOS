#ifndef __bootloader_multiCE_cpu_hpp
#define __bootloader_multiCE_cpu_hpp

#include <mm/mm.hpp>
#include <DescripTable/gdt.hpp>
#include <DescripTable/idt.hpp>

class CPU {
    IDT idt;
    MemoryManager mm;
    GDT gdt;
public:
    CPU(volatile descrptor::IDTEntry*, volatile descrptor::Entry*, int);
};

#endif