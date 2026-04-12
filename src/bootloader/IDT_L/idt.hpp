#ifndef __bootloader_IDT_L_idt_hpp
#define __bootloader_IDT_L_idt_hpp
#define _BITS_32
#include <global/type.hpp>

class IDT{
public:
#pragma pack(push, 1)
    struct IDT_item{
        u16 addr_low = 0;
        u16 selector = 0x08;
        u8 zero = 0;
        u8 type_attr = 0x8E;
        u16 addr_high = 0;
    };
#pragma pack(pop)
private:
    volatile IDT_item *idt_base;
public:
    IDT(volatile IDT_item *);
    ~IDT() = default;

    void regist(void (*handler)(void), unsigned internum, unsigned char type = 0x8E, unsigned short sec = 0x08) volatile;

    bool had_handler(unsigned i) const;

    void set_PIC() volatile;

public:
    void idtt() volatile;
};

extern unsigned long long _time_count;
extern volatile IDT::IDT_item _IDT[256];

#endif