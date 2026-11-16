#include "_cpu.hpp"
#include <id/id.hpp>
#pragma GCC optimize("Os")

namespace arch {

x86::x86(): CPU{}, mm{}, idt{new (mm) descrptor::IDTEntry[256]}, 
gdt{new (mm) descrptor::GDTEntry[256]{}, 256, idt}, tss{gdt, mm}, ldt{},
context{mm}, scheduler{context, context.create_thread(
    nullptr, 0, nullptr, 0, 0, 0, 0
)}, info{ 
    .arch_type = CPUInfo::ArchType::x86,
    .cpu_nums = 1,
    .bit_num = 32,
    .Segmentation_support = 1,
    .paging_support = 1,
    .Segmentation_mode = 1,
    .paging_mode = 0,
    .FPU_suppport = 1,
}{
    auto per_cpu_seg = gdt.regist(new (mm) char[1024], 1024, descrptor::DType::Read_write, 0);
    asm volatile (
        "mov %0, %%eax\n"
        "mov %%eax, %%fs\n"
        "movl %1, %%fs:0\n"
        "movl %2, %%fs:4\n"
        "movl %3, %%fs:8\n"
        :
        : "r"(per_cpu_seg), "r"(this), "r"(&scheduler), "r"(scheduler.current())
        : "eax", "memory"
    );

    interrupt_distributor.append_cpu(*this);

    DebugInfo (
        kout << " :: CPU[";
        kout.hex((u16)get_id());
        kout << "] >\n";
    )
}

CPUInfo x86::get_info() {
    return info;
}

void x86::preempt() { context.start_preemption(); }
void x86::collabora() { context.close_preemption(); }

void x86::run(
    void (*func)(size_t, void *), size_t argc, void *argv, size_t time, 
    u8 ring, size_t cs, size_t ds, size_t gs, size_t fs
) {
    TCB *tcb;

    if (ring > 3) {
        tcb = context.create_thread((void *)func, argc, argv, cs, ds, gs, fs);
        scheduler.enqueue(tcb, (u16)time);
    } else {
        tcb = context.create_thread(
            (void *)func, argc, argv,
            ring == 0 ? 0x08 : 0x1B,
            ring == 0 ? 0x10 : 0x23,
            ring == 0 ? 0x10 : 0x23,
            ring == 0 ? 0x30 : 0x33
        );
        scheduler.enqueue(tcb, (u16)(time - 1));
    }
}

bool arch::x86::regist_int_handler(
    void (*handler)(), unsigned int internum, arch::CPU::AccessControl access
) {
    kout << "[CPU] +int handler:" << internum << '\n';
    if (internum >= 256) return false;
    u8 dpl = (access == AccessControl::Public) ? 3 : 0;
    idt.regist(handler, internum, 0x08, dpl, true);
    return true;
}

} // namespace arch 