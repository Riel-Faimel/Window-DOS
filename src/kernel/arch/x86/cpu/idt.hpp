#ifndef __kernel_DescripTable_idt_hpp
#define __kernel_DescripTable_idt_hpp

#include <global/type.hpp>
#include "descrptor.hpp"

namespace arch {

class IDT{
    using IDT_item = descrptor::IDTEntry;

    volatile IDT_item *idt_base;
public:
    IDT(volatile IDT_item *);
    ~IDT() = default;

    // CPU exceptions must stay ring0.  Public software vectors (the native
    // API) pass DPL=3 explicitly through x86::regist_int_handler.
    void regist(
        void (*handler)(void), unsigned internum, 
        unsigned short sec = 0x08, unsigned char DPL = 0, 
        bool is_32_bits_or_16_bits = true
    ) volatile;
    bool had_handler(unsigned i) const;
};

}

#endif