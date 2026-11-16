#include "_tss.hpp"

namespace arch {
    
__attribute__((optimize("O0")))
TSM::TSM(GDT &gdt, MemoryManager& mm) {
    // The ring0 stack grows down from esp0, so esp0 must be the top of the
    // allocated block, not its base.
    auto pkg = *physicalpage.aloc(mm::PageNum{512}, true).begin();
    u32 stack_top = static_cast<u32>(static_cast<mm::Pointer<int>>(
        pkg.base + static_cast<mm::ByteNum>(pkg.pages)
    ));

    auto tss_id = gdt.create_tss(new (mm) TSS32 {
        .esp0 = stack_top,
        .ss0 = 0x10
    }, sizeof(TSS32)-1, descrptor::GateType::TSS_32_free, 0, false);
    asm volatile (
        "ltr %0\n"
        :
        : "r"(tss_id)
        : "memory"
    );
    DebugInfo(kout << ", tss: " << (u8)tss_id;)
}

} // namespace arch 