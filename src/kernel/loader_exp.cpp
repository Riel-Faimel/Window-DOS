#include "loader.hpp"
extern MemoryManager *global_heap;

extern void f(int);
iostream kout;

__attribute__((optimize("O0"), noreturn))
void LoaderMain(){
    MemoryManager init_global_heap{false};
    global_heap = &init_global_heap;
    arch::CPU cpu0{};
    cpu0.preempt();
    asm volatile ("sti");

    //===INFO===
    kout << "==============\n"
         << "renew-DOS v0.2\n"
         << "==============\n"
         << "By Riel Faimel\n";

    // init work running on CPU0
    ClusterDeviceManager cdm{};
    PeripheralDeviceSpace pds{};
    KernelModule kernelmodule;
    TaskManager taskmgr;
    kout << "[BOOT] start INIT.COM\n";
    ASMLabelMark(LoaderMain_StartB_INIT_EXE)
    auto err = taskmgr.exec("B:\\INIT.COM", 0, nullptr);
    switch (err) {
    case FexecExceptionCode::NoException: {
        _Sexit(); // init thread sleep
        __builtin_unreachable();
    } break;
    case FexecExceptionCode::OpenFileFailed: {
        kout << "[BOOT] <Exception> \"B:\\INIT.COM\" not found\n";
    } break;
    case FexecExceptionCode::DLSFault: {
        kout << "[BOOT] <Fault> File System Fault\n";
    } break;
    case FexecExceptionCode::NoExecutable: {
        kout << "[BOOT] <Fault> \"B:\\INIT.COM\" can execute\n";
    } break;
    default: {
        kout << "[BOOT] <Fault> Bad Fault\n";
    } break;
    }
    // Bad
    // I need Center Shell to keep user's control
    while(1) { asm volatile ("hlt"); };
    __builtin_unreachable();
}
