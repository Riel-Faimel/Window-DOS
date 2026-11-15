#include "_id.hpp"
#include <HL/container>

InterruptDistributor interrupt_distributor;

u8 InterruptDistributor::PIC::read(u16 port) {
    u8 value;
    asm volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void InterruptDistributor::PIC::write(u16 port, u8 value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void InterruptDistributor::PIC::io_wait() {
    write(0x80, 0);
}

void InterruptDistributor::PIC::remap(u8 master_base, u8 slave_base) {
    // ICW1: 边沿触发、级联、需要 ICW4
    write(master_cmd, 0x11);
    write(slave_cmd, 0x11);
    io_wait();

    // ICW2: 中断向量基址
    write(master_data, master_base);
    write(slave_data, slave_base);
    io_wait();

    // ICW3: master 的 IRQ2 接 slave；slave 的 cascade 是 IRQ2
    write(master_data, 0x04);
    write(slave_data, 0x02);
    io_wait();

    // ICW4: 8086 模式
    write(master_data, 0x01);
    write(slave_data, 0x01);
    io_wait();

    // 初始化后先全屏蔽，等 reg_irq 装完 handler 再逐线打开。
    mask_all();
}

void InterruptDistributor::PIC::mask_all() {
    master_mask = 0xFF;
    slave_mask = 0xFF;
    write(master_data, master_mask);
    write(slave_data, slave_mask);
}

void InterruptDistributor::PIC::mask(IRQ irq) {
    unsigned n = (unsigned)irq;
    if (n < 8) {
        master_mask |= (u8)(1u << n);
        write(master_data, master_mask);
    } else {
        slave_mask |= (u8)(1u << (n - 8));
        write(slave_data, slave_mask);
    }
}

void InterruptDistributor::PIC::unmask(IRQ irq) {
    unsigned n = (unsigned)irq;
    if (n < 8) {
        master_mask &= (u8)~(1u << n);
        write(master_data, master_mask);
    } else {
        slave_mask &= (u8)~(1u << (n - 8));
        write(slave_data, slave_mask);

        // 从片中断要经过 master 的 cascade IRQ2。
        master_mask &= (u8)~(1u << 2);
        write(master_data, master_mask);
    }
}

void InterruptDistributor::PIC::eoi(IRQ irq) {
    if ((unsigned)irq >= 8) write(slave_cmd, 0x20);
    write(master_cmd, 0x20);
}

u16 InterruptDistributor::PIC::irr() const {
    return read(master_cmd) | ((u16)read(slave_cmd) << 8);
}

u16 InterruptDistributor::PIC::isr() const {
    // OCW3: RR=1, RIS=1 -> 下一次读 command 口返回 ISR
    write(master_cmd, 0x0B);
    write(slave_cmd, 0x0B);
    u16 value = read(master_cmd) | ((u16)read(slave_cmd) << 8);
    // 恢复默认的 IRR 读模式
    write(master_cmd, 0x0A);
    write(slave_cmd, 0x0A);
    return value;
}

u16 InterruptDistributor::PIC::imr() const {
    return read(master_data) | ((u16)read(slave_data) << 8);
}



InterruptDistributor::InterruptDistributor() {
    pic.remap();
}

void InterruptDistributor::reg_int(void (*handler)(), unsigned internum, arch::CPUIndex cpuid) {
    if (!handler) return;

    bool known = false;
    for (auto &entry : int_entries) {
        if (entry.handler == handler && entry.vector == internum) {
            known = true;
            break;
        }
    }
    if (!known)
        int_entries.append(IntEntry{handler, internum});

    // internum is the absolute vector (e.g. 0x21), no IRQ base added
    if (cpuid == arch::CPUIndex{0xFFFF}) 
    for (auto &cpu : cpu_list) {
        cpu.regist_int_handler(handler, internum, arch::CPU::AccessControl::Public);
    }
    else
    for (auto &cpu : cpu_list)
    if (cpu.get_id() == cpuid) {
        cpu.regist_int_handler(handler, internum, arch::CPU::AccessControl::Public);
    }
}

void InterruptDistributor::reg_irq(void (*handler)(), IRQ irq_num, arch::CPUIndex cpuid) {
    bool known = false;
    for (auto &entry : irq_entries) {
        if (entry.handler == handler && entry.irq == irq_num) {
            known = true;
            break;
        }
    }
    if (!known)
        irq_entries.append(IrqEntry{handler, irq_num});

    if (cpuid == arch::CPUIndex{0xFFFF}) 
    for (auto &cpu : cpu_list) {
        cpu.regist_int_handler(handler, (unsigned char)irq_num+0x30, arch::CPU::AccessControl::HardOnly);
    }
    else
    for (auto &cpu : cpu_list)
    if (cpu.get_id() == cpuid) {
        cpu.regist_int_handler(handler, (unsigned char)irq_num+0x30, arch::CPU::AccessControl::HardOnly);
    }

    // handler 已进 CPU 的 IDT；master 线可以直接开。
    // 从片线在 cascade IRQ2 的 handler 落地之前保持屏蔽，
    // 否则 CPU 会先取 0x32 而 #GP。
    if ((unsigned)irq_num < 8)
        pic.unmask(irq_num);
    else
        pic.mask(irq_num);
}

void InterruptDistributor::append_cpu(arch::CPU &cpu) {
    // 新 CPU 上线时，把已注册的全局软中断和 IRQ 回放到它的 IDT。
    for (auto &entry : int_entries) {
        cpu.regist_int_handler(
            entry.handler,
            entry.vector,
            arch::CPU::AccessControl::Public
        );
    }

    for (auto &entry : irq_entries) {
        cpu.regist_int_handler(
            entry.handler,
            (unsigned char)entry.irq + 0x30,
            arch::CPU::AccessControl::HardOnly
        );
    }
}
