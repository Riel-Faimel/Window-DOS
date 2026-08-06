#ifndef __bootloader_DescrpTable_idt_hpp
#define __bootloader_DescrpTable_idt_hpp

#include <global/type.hpp>
#include <DescrpTable/idt_.hpp>
#include "descrptor.hpp"

class IDT{
    using IDT_item = descrptor::IDTEntry;

    volatile IDT_item *idt_base;
public:
    IDT(volatile IDT_item *);
    ~IDT() = default;

    void regist(void (*handler)(void), unsigned internum, unsigned char type = 0x8E, unsigned short sec = 0x08) volatile;

    bool had_handler(unsigned i) const;

    void set_PIC() volatile;
};

extern unsigned long long _time_count;
extern volatile descrptor::IDTEntry _IDT[256];

#endif