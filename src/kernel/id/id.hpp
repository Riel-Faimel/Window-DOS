#ifndef __kernel_DescripTable_id_hpp
#define __kernel_DescripTable_id_hpp

#include <global/type.hpp>
#include <arch/cpu.hpp>

enum class IRQ : unsigned char {
    time = 0,
    keyboard = 1,
    COM1 = 3,
    COM2 = 4,
    PS_2 = 12,
    ATA_Master = 14,
    ATA_Slave = 15,
};

class InterruptDistributor {
public:
    InterruptDistributor();
    void reg_irq(void (*handler)(), IRQ irq_num, size_t cpuid = 0);
    void reg_idt(arch::IDT idt);
};

extern InterruptDistributor interrupt_distributor;

#endif