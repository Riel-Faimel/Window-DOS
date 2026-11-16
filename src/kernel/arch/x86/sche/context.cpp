#include "context.hpp"

#include "tcb.hpp"

#include <global/new.hpp>
#include <id/id.hpp>
#include <mm/mm.hpp>

extern "C" {
    void _Sexit();
    void _Sint_sche();
    void basic_time_handler_c();
}

namespace arch {

X86Context::X86Context(MemoryManager &mm) : mm{mm} {
    // _Sexit is the software vector the scheduler uses to enter exit.
    interrupt_distributor.reg_int(&_Sexit, 0x20);
}

void X86TCB::tcb_push(TCBField field, size_t value) {
    frame.xsp -= 4;

    switch (field) {
    case TCBField::flags:
        *(size_t *)frame.xsp = frame.xflag;
        break;
    case TCBField::ip:
        *(size_t *)frame.xsp = frame.xip;
        break;
    case TCBField::word:
        *(size_t *)frame.xsp = value;
        break;
    }
}

void X86TCB::tcb_set_ip(size_t ip) {
    frame.xip = ip;
}

TCB *X86Context::create_thread(
    void *func, size_t argc, void *argv,
    size_t code_seg, size_t data_seg, size_t gs, size_t fs
) {
    auto *tcb = new (mm) X86TCB{};

    X86Frame &frame = tcb->frame;

    frame.context.xdx = (size_t)argv;
    frame.context.xax = argc;
    frame.xip = (size_t)func;
    frame.xcs = code_seg & 0xFFFF;
    frame.xsp = (size_t)new (mm) char[4069];
    frame.xss = data_seg & 0xFFFF;
    frame.xfs = fs & 0xFFFF;
    frame.xds = data_seg & 0xFFFF;
    frame.xes = data_seg & 0xFFFF;
    frame.xgs = gs & 0xFFFF;

    return tcb;
}

void X86Context::destroy(TCB *tcb) {
    auto *x86tcb = static_cast<X86TCB *>(tcb);
    auto *frame = &x86tcb->frame;

    if (frame->xsp) operator delete ((void *)frame->xsp, mm);
    operator delete (x86tcb, mm);
}

[[noreturn]] void X86Context::switch_to(TCB &next) {
    auto *x86tcb = static_cast<X86TCB *>(&next);
    X86Frame *frame = &x86tcb->frame;

    if ((frame->xcs & 3) == 0) {
        // ring0
        asm volatile (
            "movl %0, %%fs:8\n"
            "mov %1, %%esp\n"
            "mov %2, %%eax\n"
            "jmp _Sring0_back\n"
            :
            : "r"(&next), "r"(frame->xsp), "r"(&next)
            : "eax", "memory"
        );
    } else {
        // ring3
        asm volatile (
            "movl %0, %%fs:8\n"
            "mov %1, %%esp\n"
            "jmp _Sring3_back\n"
            :
            : "r"(&next), "r"(&frame->xds)
            : "memory"
        );
    }

    __builtin_unreachable();
}

void X86Context::idle() {
    asm volatile ("hlt\n");
}

void X86Context::start_preemption() {
    interrupt_distributor.reg_irq(&_Sint_sche, IRQ::time);
}

void X86Context::close_preemption() {
    interrupt_distributor.reg_irq(&basic_time_handler_c, IRQ::time);
}

} // namespace arch
