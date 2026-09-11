#ifndef __bootloader_multiCE_cpu_hpp
#define __bootloader_multiCE_cpu_hpp

#include <global/type.hpp>
#include <arch/arch_type>
#include <TL/utility>
#include <mm/mm.hpp>
#include "gdt.hpp"
#include "idt.hpp"
#include "tss.hpp"
#include <multiCE/default/cs.hpp>
#include <syscall/DOS/DOScall.hpp>

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
    CPUInfo info;

    CPU();
    auto next_node(){ return next; }

    CPUInfo get_info();

    // memory mode control
    void paging();
    void unpaging();

    // scheduler control
    void preempt();
    void collabora();
    void run(
        void (*)(size_t, void *), size_t, void *, size_t time = 1, u8 ring = 3,
        size_t cs = 0, size_t ds = 0, size_t gs = 0, size_t fs = 0
    );

    // interrupt regist
    enum class AccessControl : u8 {
        Public, Private, HardOnly
    };
    bool regist_int_handler(void (*handler)(void), unsigned internum, AccessControl access);

    // segmention control
    const GDT &get_gdt() const { return gdt; }
};
#pragma pack(pop)

extern CPU *cpu_list_root;

#endif