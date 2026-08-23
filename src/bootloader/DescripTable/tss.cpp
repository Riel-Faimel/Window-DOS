#include "_tss.hpp"


TSM::TSM(GDT &gdt) {
    auto tss_id = gdt.create_tss(new TSS32 {
        .esp0 = (physicalpage.aloc(512, true).begin()).operator*().address.low,
        .ss0 = 0x10
    }, sizeof(TSS32)-1, descrptor::GateType::TSS_32_free, 0, false);
    asm volatile (
        "ltr %0\n"
        :
        : "r"(tss_id)
        : "memory"
    );
#ifdef _DEBUG
    cout << ", tss: " << (u8)tss_id;
#endif
}