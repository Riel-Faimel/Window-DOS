#ifndef __bootloader_mm_gdt_hpp
#define __bootloader_mm_gdt_hpp

#include <global/type.hpp>
#include <DescripTable/idt.hpp>
#include "descrptor.hpp"

class GDT {
    using GDTEntry = descrptor::Entry;
    using GDTType = descrptor::DTType;
    using GateType = descrptor::GateType;

    #pragma pack(push, 1)
    struct GDTPtr {
        u16 limit;
        u32 base;
    };

    volatile GDTEntry *entries;
    u16 limit;
    #pragma pack(pop)
public:
    GDT(volatile GDTEntry *, u16, IDT&);
    ~GDT() = default;
    //true for one byte, false for 4KB
    unsigned regist(
        void *Segment_base, u32 Segment_limit, GDTType Type, u8 ring, 
        bool unit_of_1bit_or_4KB = true, bool is32_or16 = true, 
        bool is_64_long_mode = false, bool isnot_System_segment = true, bool AVL = false
    ) volatile;

    unsigned create_a_gate(
        void *offset, u16 Segment, GateType type,
        u8 params_count, u8 ring = 0x00
    ) volatile;

    unsigned create_tss(
        void *base_addr, u32 seg_lim, GateType type,
        u8 ring, bool open_4k_granularity
    ) volatile;
};
#endif