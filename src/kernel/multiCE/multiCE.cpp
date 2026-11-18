#include "_multiCE.hpp"

extern "C" { multiCE Multi_Channel_executor; }

multiCE::multiCE() {}

void multiCE::cut(size_t /*cpuid*/, TCB */*tid*/, size_t /*intnum*/) {
    ;
}

TCB *multiCE::run(
    void (*func)(size_t, void *), size_t argc, void *argv, size_t time, 
    u8 ring, size_t cs, size_t ds, size_t gs, size_t fs
) {
    for (auto &cpu : cpu_list)
    if (true) {
        return cpu.run(
            func, argc, argv, time,
            ring, cs, ds, gs, fs
        );
    }
    return nullptr;
}