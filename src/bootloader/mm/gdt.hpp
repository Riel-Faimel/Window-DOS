#ifndef __bootloader_mm_gdt_hpp
#define __bootloader_mm_gdt_hpp

#include <global/type.hpp>
#include <IDT_L/idt.hpp>
class GDT {
#pragma pack(push, 1)
public:
    enum class GDTType : u8 {
        Read = 0x0,
        Read_accessed = 0x1,
        Read_write = 0x2,
        Read_write_accessed = 0x3,
        Read_expand_down = 0x4,
        Read_expand_down_accessed = 0x5,
        Read_write_expand_down = 0x6,
        Read_write_expand_down_accessed = 0x7,

        Execute = 0x8,
        Execute_accessed = 0x9,
        Execute_read = 0xA,
        Execute_read_accessed = 0xB,
        Execute_conforming = 0xC,
        Execute_conforming_accessed = 0xD,  
        Execute_read_conforming = 0xE,
        Execute_read_conforming_accessed = 0xF
    };
    /**
     * ACCESSED: accessed means the segment has been accessed by the CPU. 
     * This bit is set by the CPU when the segment is accessed, 
     * and can be used by the operating system to track which segments are in use.
     * READWRITE: read/write indicates whether the segment is readable or writable. 
     * For code segments, this bit is set to 1 if the segment is readable, and for data segments, 
     * this bit is set to 1 if the segment is writable.
     * EXPANDDOWN: expand down indicates whether the segment grows downwards (towards lower memory addresses)
     * CONFORMING: conforming indicates whether the segment can be executed from a lower privilege level. 
     * If this bit is set, the segment can be executed from any privilege level,
     */

    enum class GateType : u8{
        TSS_16_free = 1,
        LDT = 2,
        TSS_16_busy = 3,
        Call_Gate_16 = 4,
        Task_Gate = 5,
        INT_Gate_16 = 6,
        Trap_Gate_16 = 7,
        TSS_32_free = 9,
        TSS_32_busy = 11,
        Call_Gate_32 = 12,
        INT_Gate_32 = 14,
        Trap_Gate_32 = 15,
    };
union GDTEntry{
    struct {
        u16 Segment_limit_low;
        u16 Base_address_low;
        u8 Base_address_middle;
        GDTType Type : 4;
        u8 not_a_system_descriptor : 1;
        u8 ring : 2;
        u8 exist_segment : 1;
        u8 Segment_limit_high : 4;
        u8 AVL : 1;
        u8 is_64_long_mode : 1;
        u8 is32_or16 : 1;
        u8 unit_of_1bit_or_4KB : 1;
        u8 Base_address_high;
    }Segment_Descript;

    struct {
        u16 Offset_address_low;
        u16 Segment_selector;
        u8 params_count : 5;
        u8 zero : 3;
        GateType Type : 4;
        u8 not_a_system_descriptor : 1;
        u8 ring : 2;
        u8 exist_Segment : 1;
        u16 Offset_address_high;
    }Call_Gate;

    struct {
        u16 Seg_limit_low;
        u16 Base_Address_low;
        u8 Base_Address_mid;
        GateType Type : 4;
        u8 nota_system_seg : 1;
        u8 ring : 2;
        u8 exist_Segment : 1;
        u8 Seg_limit_high : 4;
        u8 AVL : 1;
        u8 must_zero : 2;
        u8 G : 1;
        u8 Base_Address_high;
    } TSS;
};
private:
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