#include "_cpu.hpp"
#pragma GCC optimize("Os")

namespace arch {

x86::x86(): CPU{}, mm{}, idt{new (mm) descrptor::IDTEntry[256]}, 
gdt{new (mm) descrptor::GDTEntry[256]{}, 256, idt}, tss{gdt, mm}, 
scheduler{mm, idt}, info{ 
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
        :
        : "r"(per_cpu_seg), "r"(this)
        : "eax", "memory"
    );

    DebugInfo (
        kout << " :: CPU[";
        kout.hex((u16)get_id());
        kout << "] >\n";
    )
}

CPUInfo x86::get_info() {
    return info;
}

void x86::preempt() { scheduler.start_preemption(idt); }
void x86::collabora() { scheduler.close_preemption(idt); }

void x86::run(
    void (*func)(size_t, void *), size_t argc, void *argv, size_t time, 
    u8 ring, size_t cs, size_t ds, size_t gs, size_t fs
) {
    if (ring > 3)
    scheduler.run((void *)func, argc, argv, time, cs, ds, gs, fs);
    else {
        scheduler.run(
            (void *)func, argc, argv, time-1,
            ring==0?0x08:0x1B, ring==0?0x10:0x23,
            ring==0?0x10:0x23, ring==0?0x30:0x33
        );
    }
}

bool arch::x86::regist_int_handler(
    void (*handler)(), unsigned int internum, arch::CPU::AccessControl access
) {
    kout << "[CPU] +handler:" << internum << '\n';
    if (internum >= 256) return false;
    u8 dpl = (access == AccessControl::Public) ? 3 : 0;
    idt.regist(handler, internum, 0x08, dpl, true);
    return true;
}

extern "C" { extern constexpr int soff = __builtin_offsetof(x86, scheduler); }

} // namespace arch 