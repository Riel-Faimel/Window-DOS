#ifndef __bootloader_DescripTable_apic_and_intdis_hpp
#define __bootloader_DescripTable_apic_and_intdis_hpp

class IDT;

enum class IRQ : unsigned char {
    time = 0,
    keyboard = 1,
    COM1 = 3,
    COM2 = 4,
    PS_2 = 12,
    ATA_Master = 13,
    ATA_Slave = 14,
};

class intMgr {
public:
    intMgr();
    void reg_irq(void (*handler)(), IRQ irq_num, int cpuid = 0);
    void reg_idt(IDT idt);
};

extern intMgr interrupt_distributor;

#endif