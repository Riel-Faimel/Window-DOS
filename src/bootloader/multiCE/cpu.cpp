#include "_cpu.hpp"

CPU *cpu_list_root;
atomic<bool> countorlock{false};
size_t cpuidcountor{0};

CPU::CPU(): mm{}, idt{new descrptor::IDTEntry[256]}, 
gdt{new descrptor::GDTEntry[256]{}, 256, idt}, tss{gdt}, scheduler{}{
    while(!countorlock.try_lock());
    {
        auto &cpup = cpu_list_root;
        for (;cpup != nullptr;cpup = cpup->next);
        cpup = this;
        cpuidcountor++;

        auto per_cpu_seg = gdt.regist(new char[1024], 1024, descrptor::DType::Read_write, 0);
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

extern "C" { 
   extern constexpr int soff = sizeof(MemoryManager) + sizeof(IDT)
   + sizeof(GDT) + sizeof(TSM) + sizeof(void *)*2;
}