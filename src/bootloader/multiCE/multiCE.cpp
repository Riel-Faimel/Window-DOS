#include "_multiCE.hpp"

extern "C" { multiCE Multi_Channel_executor; }

multiCE::multiCE() {}

void multiCE::cut(size_t cpuid, TCB *tid, size_t intnum) {}

void multiCE::run(
    void (*func)(size_t, void *), size_t argc, void *argv, size_t time, 
    u8 ring, size_t cs, size_t ds, size_t gs, size_t fs
) {
    for (auto &cpu : rtl::list_tranveser{cpu_list_root})
    if (true) {
        cpu.run(
            func, argc, argv, time,
            ring==0?0x08:0x1B, ring==0?0x10:0x23,
            ring==0?0x10:0x23, ring==0?0x30:0x33
        );
        return;
    }
}