#ifndef __bootloader_DescripTable_idt_hpp
#define __bootloader_DescripTable_idt_hpp

#include <global/type.hpp>
#include "descrptor.hpp"

class IDT{
    using IDT_item = descrptor::IDTEntry;

    volatile IDT_item *idt_base;
public:
    IDT(volatile IDT_item *);
    ~IDT() = default;

    void regist(
        void (*handler)(void), unsigned internum, 
        unsigned short sec = 0x08, unsigned char DPL = 3, 
        bool is_32_bits_or_16_bits = true
    ) volatile;
    bool had_handler(unsigned i) const;
};

#endif