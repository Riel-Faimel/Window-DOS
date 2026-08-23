#ifndef __bootloader_TM_tss_hpp
#define __bootloader_TM_tss_hpp

#include <global/type.hpp>
class GDT;

struct TSS32 {
    u16 task_link = 0; u16 _r1; u32 esp0 = 0; u16 ss0 = 0; u16 _r2;
    u32 esp1 = 0; u16 ss1 = 0; u16 _r3; u32 esp2 = 0; u16 ss2 = 0; u16 _r4;
    u32 cr3 = 0; u32 eip; u32 eflags; u32 eax; u32 ecx; u32 edx;
    u32 ebx; u32 esp; u32 ebp; u32 esi; u32 edi; u16 es; u16 _r5;
    u16 cs; u16 _r6; u16 ss; u16 _r7; u16 ds; u16 _r8;
    u16 fs; u16 _r9; u16 gs; u32 _r10; u16 IOMapBaseAddr;
};

struct TSM {
    TSM(GDT &gdt);
};


#endif