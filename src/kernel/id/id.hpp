#ifndef __kernel_DescripTable_id_hpp
#define __kernel_DescripTable_id_hpp

#include <global/type.hpp>
#include <arch/cpu.hpp>
#include <HL/container>

enum class IRQ : unsigned char {
    time = 0,
    keyboard = 1,
    cascade = 2,
    COM1 = 3,
    COM2 = 4,
    PS_2 = 12,
    ATA_Master = 14,
    ATA_Slave = 15,
};

class InterruptDistributor {
    
class PIC {
public:
    static constexpr u16 master_cmd = 0x20;
    static constexpr u16 master_data = 0x21;
    static constexpr u16 slave_cmd = 0xA0;
    static constexpr u16 slave_data = 0xA1;

    // 重映射到 master_base / slave_base，并先把所有线屏蔽。
    void remap(u8 master_base = 0x30, u8 slave_base = 0x38);

    void mask_all();
    void mask(IRQ irq);
    void unmask(IRQ irq);
    void eoi(IRQ irq);

    u16 irr() const;
    u16 isr() const;
    u16 imr() const;

private:
    static u8 read(u16 port);
    static void write(u16 port, u8 value);
    static void io_wait();

    u8 master_mask = 0xFF;
    u8 slave_mask = 0xFF;
} pic;

    struct IrqEntry {
        void (*handler)();
        IRQ irq;
    };

    struct IntEntry {
        void (*handler)();
        unsigned vector;
    };

    rtl::list<IrqEntry> irq_entries;
    rtl::list<IntEntry> int_entries;

public:
    InterruptDistributor();
    void reg_int(void (*handler)(), unsigned irq_num, arch::CPUIndex cpuid = arch::CPUIndex{0xFFFF});
    void reg_irq(void (*handler)(), IRQ irq_num, arch::CPUIndex cpuid = arch::CPUIndex{0xFFFF});
    void append_cpu(arch::CPU &cpu);
};

extern InterruptDistributor interrupt_distributor;

#endif