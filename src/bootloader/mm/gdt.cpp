#include "gdt.hpp"
#include <IDT_L/IDNT.hpp>
#include <drv/screen/screen_srv.hpp>

import lib32;

extern "C" void __attribute__((regparm(1)))fresh_gdt(void *gdt_ptr);

void NP_handler(){
    screen->print("[ERROR] No segment present\n");
    qps("[ERROR] No segment present\n");
    while(1){asm volatile ("hlt");}
};

void SS_handler(){
    screen->print("[ERROR] Stack segment fault\n");
    qps("[ERROR] Stack segment fault\n");
    while(1){asm volatile ("hlt");}
};

GDT::GDT(GDTEntry *entries_init, u16 limit_init, IDT& idt) :
entries(entries_init), limit(limit_init) {
    idt.regist(&NP_handler, static_cast<unsigned>(IDNT::_NP));
    idt.regist(&SS_handler, static_cast<unsigned>(IDNT::_SS));
    GDTPtr gdt_ptr;

    entries[0] = {};
    regist(nullptr, 0xFFFFF, GDTType::Read_write, 0);
    regist(nullptr, 0xFFFFF, GDTType::Execute_read, 0);

    print_hex(reinterpret_cast<unsigned>(entries));

    gdt_ptr.limit = sizeof(GDTEntry) * limit - 1;
    gdt_ptr.base = reinterpret_cast<u32>(entries);
    //while(1){asm volatile ("hlt");}

    fresh_gdt(&gdt_ptr);
}

inline void GDT::regist(
    void *Segment_base, u32 Segment_limit, GDT::GDTType Type, u8 ring, 
    bool unit_of_1bit_or_4KB, bool is32_or16, bool is_64_long_mode, 
    bool isnot_System_segment, bool AVL
){
    for(unsigned i = 1; i < limit; i++){
        if(entries[i].exist_segment)continue;
        entries[i].Segment_limit_low = reinterpret_cast<u32>(Segment_limit) & 0xFFFF;
        entries[i].Base_address_low = reinterpret_cast<u32>(Segment_base) & 0xFFFF;
        entries[i].Base_address_middle = (reinterpret_cast<u32>(Segment_base) >> 16) & 0xFF;
        entries[i].Type = Type;
        entries[i].isnot_System_segment = isnot_System_segment ? 1 : 0;
        entries[i].ring = ring;
        entries[i].exist_segment = 1;
        entries[i].Segment_limit_high = (reinterpret_cast<u32>(Segment_limit) >> 16) & 0xF;
        entries[i].AVL = AVL;
        entries[i].is_64_long_mode = is_64_long_mode ? 1 : 0;
        entries[i].is32_or16 = is32_or16 ? 1 : 0;
        entries[i].unit_of_1bit_or_4KB = unit_of_1bit_or_4KB ? 1 : 0;
        entries[i].Base_address_high = (reinterpret_cast<u32>(Segment_base) >> 24) & 0xFF;
        return;
    }
}