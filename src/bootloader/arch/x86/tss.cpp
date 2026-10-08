#include "_tss.hpp"


__attribute__((optimize("O0")))
TSM::TSM(GDT &gdt, MemoryManager& mm) {
    auto tss_id = gdt.create_tss(new (mm) TSS32 {
        .esp0 = (u32)static_cast<mm::Pointer<int>>((physicalpage.aloc(mm::PageNum{512}, true).begin()).operator*().base),
        .ss0 = 0x10
    }, sizeof(TSS32)-1, descrptor::GateType::TSS_32_free, 0, false);
    asm volatile (
        "ltr %0\n"
        :
        : "r"(tss_id)
        : "memory"
    );
    DebugInfo(cout << ", tss: " << (u8)tss_id;)
}