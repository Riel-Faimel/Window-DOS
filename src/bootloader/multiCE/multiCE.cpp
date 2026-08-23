#include "_multiCE.hpp"

extern "C" { multiCE Multi_Channel_executor; }

multiCE::multiCE() {}

void multiCE::cut(size_t cpuid, TCB *tid, size_t intnum) {}

void multiCE::run(void *func, size_t argc, void *argv, u8 ring, size_t time) {
    for (auto &cpu : rtl::list_tranveser{cpu_list_root}) {
        if (true) {
            cpu.scheduler.run(func, argc, argv, ring, time);
            return;
        }
    }
}