#include "gdt.hpp"
#include <DescrpTable/IDNT.hpp>
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

GDT::GDT(volatile GDTEntry *entries_init, u16 limit_init, IDT& idt) :
entries(entries_init), limit(limit_init) {
    idt.regist(&NP_handler, static_cast<unsigned>(IDNT::_NP));
    idt.regist(&SS_handler, static_cast<unsigned>(IDNT::_SS));
    GDTPtr gdt_ptr;

    for(unsigned i = 0;i < 8;i++){
        reinterpret_cast<volatile unsigned char *>(entries)[i] = 0;
    }
    regist(nullptr, 0xFFFFF, GDTType::Execute_read, 0); //must 0x08
    regist(nullptr, 0xFFFFF, GDTType::Read_write, 0); //must 0x10
    regist(nullptr, 0xFFFFF, GDTType::Execute_read, 0, false, false);
    regist(nullptr, 0xFFFFF, GDTType::Read_write, 0, false, false);

    gdt_ptr.limit = sizeof(GDTEntry) * limit - 1;
    gdt_ptr.base = reinterpret_cast<u32>(entries);

    /*
    screen->print("GDT in: ");
    print_hex(reinterpret_cast<unsigned>(entries));
    print_char('\n');
    //*/

    fresh_gdt(&gdt_ptr);
}

__attribute__((optimize("O0")))
unsigned GDT::regist(
    void *Segment_base, u32 Segment_limit, GDT::GDTType Type, u8 ring, 
    bool unit_of_1bit_or_4KB, bool is32_or16, bool is_64_long_mode, 
    bool isnot_System_segment, bool AVL
) volatile {
    for(unsigned i = 1; i < limit; i++){
        if(entries[i].Segment_Descript.exist_segment)continue;
        entries[i].Segment_Descript.Segment_limit_low = reinterpret_cast<u32>(Segment_limit) & 0xFFFF;
        entries[i].Segment_Descript.Base_address_low = reinterpret_cast<u32>(Segment_base) & 0xFFFF;
        entries[i].Segment_Descript.Base_address_middle = (reinterpret_cast<u32>(Segment_base) >> 16) & 0xFF;
        entries[i].Segment_Descript.Type = Type;
        entries[i].Segment_Descript.not_a_system_descriptor = isnot_System_segment ? 1 : 0;
        entries[i].Segment_Descript.ring = ring;
        entries[i].Segment_Descript.exist_segment = 1;
        entries[i].Segment_Descript.Segment_limit_high = (reinterpret_cast<u32>(Segment_limit) >> 16) & 0xF;
        entries[i].Segment_Descript.AVL = AVL;
        entries[i].Segment_Descript.is_64_long_mode = is_64_long_mode ? 1 : 0;
        entries[i].Segment_Descript.is32_or16 = is32_or16 ? 1 : 0;
        entries[i].Segment_Descript.unit_of_1bit_or_4KB = unit_of_1bit_or_4KB ? 1 : 0;
        entries[i].Segment_Descript.Base_address_high = (reinterpret_cast<u32>(Segment_base) >> 24) & 0xFF;
        return i;
    }
    return -1;
}

__attribute__((optimize("O0")))unsigned int GDT::create_a_gate(
    void *offset, u16 Segment, GateType type, u8 params_count, u8 ring
) volatile {
    for(unsigned i = 1;i < limit;i++){
        if(entries[i].Call_Gate.exist_Segment)continue;
        entries[i].Call_Gate.exist_Segment = 1;
        entries[i].Call_Gate.not_a_system_descriptor = 0;
        entries[i].Call_Gate.Offset_address_high = (reinterpret_cast<u32>(offset) >> 16) & 0xFFFF;
        entries[i].Call_Gate.Offset_address_low = reinterpret_cast<u32>(offset) & 0xFFFF;
        entries[i].Call_Gate.params_count = params_count;
        entries[i].Call_Gate.ring = ring;
        entries[i].Call_Gate.Segment_selector = Segment;
        entries[i].Call_Gate.Type = type;
        entries[i].Call_Gate.zero = 0;
        return i;
    };
    return -1;
}

unsigned int GDT::create_tss(
    void *base_addr, u32 seg_lim, GDT::GateType type, 
    u8 ring, bool open_4k_granularity
) volatile{
    for (unsigned i = 1;i < limit;i++){
        if(entries[i].TSS.exist_Segment)continue;
        entries[i].TSS.must_zero = 0;
        entries[i].TSS.Base_Address_low = (reinterpret_cast<u32>(base_addr) & 0xFFFF);
        entries[i].TSS.Base_Address_mid = (reinterpret_cast<u32>(base_addr) >> 16) & 0xFF;
        entries[i].TSS.Base_Address_high = (reinterpret_cast<u32>(base_addr) >> 24) & 0xFFFF;
        entries[i].TSS.exist_Segment = 1;
        entries[i].TSS.G = open_4k_granularity ? 1 : 0;
        entries[i].TSS.nota_system_seg = 0;
        entries[i].TSS.ring = ring;
        entries[i].TSS.Type = type;
        entries[i].TSS.Seg_limit_low = seg_lim & 0xFFFF;
        entries[i].TSS.Seg_limit_high = (seg_lim >> 16) & 0xFF;
    };
}
