#ifndef __bootloader_run_TSS_hpp
#define __bootloader_run_TSS_hpp
#include <mm/gdt.hpp>

class TSS {
#pragma pack(1, push)
    struct TaskStateSeg {
        u16 task_link;
        u16 re_0;
        u32 esp0;
        u16 ss0;
        u16 re_1;
        u32 esp1;
        u16 ss1;
        u16 re_2;
        u32 esp2;
        u16 ss2;
        u32 CR3;
        u32 eip;
        u32 EFLAGS;
        u32 eax;
        u32 ecx;
        u32 edx;
        u32 ebx;
        u32 esp;
        u32 ebp;
        u32 esi;
        u32 edi;
        u16 es;
        u16 re_3;
        u16 cs;
        u16 re_4;
        u16 ss;
        u16 re_5;
        u16 ds;
        u16 re_6;
        u16 fs;
        u16 re_7;
        u16 gs;
        u16 re_8;
        u16 LDT_seg;
        u32 re_9;
        u16 IO_Map;
    };
#pragma pack(pop)
private:
    TaskStateSeg tss;
public:
    TSS(GDT &);

    void create_a_task();
    void delete_a_task();
};

#endif