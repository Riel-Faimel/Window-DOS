#include "_multiCE.hpp"

extern "C" { multiCE Multi_Channel_executor; }

multiCE::multiCE() {}

void multiCE::cut(size_t /*cpuid*/, TCB */*tid*/, size_t /*intnum*/) {
    ;
}

TCB *multiCE::run(
    void (*func)(size_t, void *), size_t argc, 
    void *argv, mem::MemSpace ms, size_t time, u8 ring
) {
    for (auto &cpu : cpu_list)
    if (true) return cpu.run(func, argc, argv, ms, time, ring);
    return nullptr;
}