#include "_cpu.hpp"

CPU *cpu_list_root;
atomic<bool> countorlock{false};
size_t cpuidcountor{0};

CPU::CPU(): mm{}, idt{new (mm) descrptor::IDTEntry[256]}, 
gdt{new (mm) descrptor::GDTEntry[256]{}, 256, idt}, tss{gdt, mm}, 
scheduler{mm, idt}, syscall{idt}{
    while(!countorlock.try_lock());
    {
        if (cpu_list_root == nullptr) {
            cpu_list_root = this;
        } else for (auto cpup = cpu_list_root;cpup != nullptr;cpup = cpup->next) {
            if (cpup->next == nullptr) {
                cpup->next = this;
                break;
            }
        }
        cpuidcountor++;

        auto per_cpu_seg = gdt.regist(new (mm) char[1024], 1024, descrptor::DType::Read_write, 0);
        asm volatile (
            "mov %0, %%eax\n"
            "mov %%eax, %%fs\n"
            "movl %1, %%fs:0\n"
            :
            : "r"(per_cpu_seg), "r"(this)
            : "eax", "memory"
        );
    }
    countorlock.unlock();

#ifdef _DEBUG
    cout << " :: CPU[";
    print_hex((u8)cpuid, false);
    cout << "] >\n";
#endif
}

void CPU::preempt() { scheduler.start_preemption(idt); }
void CPU::collabora() { scheduler.close_preemption(idt); }

void CPU::run(
    void (*func)(size_t, void *), size_t argc, void *argv, size_t time, 
    u8 ring, size_t cs, size_t ds, size_t gs, size_t fs
) {
    if (ring <= 3)
    scheduler.run((void *)func, argc, argv, time, cs, ds, gs, fs);
    else {
        scheduler.run(
            (void *)func, argc, argv, time-1,
            ring==0?0x08:0x1B, ring==0?0x10:0x23,
            ring==0?0x10:0x23, ring==0?0x30:0x33
        );
    }
}

extern "C" { 
   extern constexpr int soff = sizeof(MemoryManager) + sizeof(IDT)
   + sizeof(GDT) + sizeof(TSM) + sizeof(void *)*2;
}