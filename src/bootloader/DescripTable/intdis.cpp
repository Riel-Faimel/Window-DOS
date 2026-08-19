#include "_intdis.hpp"

InterruptDistributor interrupt_distributor;

__attribute__((naked)) void set_PIC() {
    asm volatile (
        "movb $0x11, %%al\n"
        "outb %%al, %0\n"
        "movb $0x11, %%al\n"
        "outb %%al, %1\n"
        "call io_wait\n"
        
        "movb $0x30, %%al\n"
        "outb %%al, %2\n"
        "movb $0x38, %%al\n"
        "outb %%al, %3\n"
        "call io_wait\n"
        
        "movb $0x04, %%al\n"
        "outb %%al, %2\n"
        "movb $0x02, %%al\n"
        "outb %%al, %3\n"
        "call io_wait\n"
        
        "movb $0x01, %%al\n"
        "outb %%al, %2\n"
        "movb $0x01, %%al\n"
        "outb %%al, %3\n"
        "call io_wait\n"

        "ret\n"
        :
        : "N" (PIC1_CMD), "N" (PIC2_CMD),
          "N" (PIC1_DATA), "N" (PIC2_DATA)
        : "al", "memory"
    );
}

InterruptDistributor::InterruptDistributor() {
    set_PIC();
}

void InterruptDistributor::reg_irq(void (*handler)(), IRQ irq_num, int cpuid) {
    for (auto &cpu : rtl::list_tranveser{cpu_list_root}) {
        if (cpu.cpuid == cpuid) {
            cpu.idt.regist(handler, (unsigned char)irq_num+0x30);
        }
    }
}

void InterruptDistributor::reg_idt(IDT idt) {
    ;
}