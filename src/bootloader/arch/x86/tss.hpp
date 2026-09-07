#ifndef __bootloader_TM_tss_hpp
#define __bootloader_TM_tss_hpp

#include <global/type.hpp>
class GDT;
struct MemoryManager;

struct TSS32 {
    u16 task_link = 0; u16 _r1 = 0; u32 esp0 = 0; u16 ss0 = 0; u16 _r2 = 0;
    u32 esp1 = 0; u16 ss1 = 0; u16 _r3 = 0; u32 esp2 = 0; u16 ss2 = 0; u16 _r4 = 0;
    u32 cr3 = 0; u32 eip = 0; u32 eflags = 0; u32 eax = 0; u32 ecx = 0; u32 edx = 0;
    u32 ebx = 0; u32 esp = 0; u32 ebp = 0; u32 esi = 0; u32 edi = 0; u16 es = 0; u16 _r5 = 0;
    u16 cs = 0; u16 _r6 = 0; u16 ss = 0; u16 _r7 = 0; u16 ds = 0; u16 _r8 = 0;
    u16 fs = 0; u16 _r9 = 0; u16 gs = 0; u32 _r10 = 0; u16 IOMapBaseAddr = 0;
};

struct TSM {
    TSM(GDT &gdt, MemoryManager&);
};


#endif