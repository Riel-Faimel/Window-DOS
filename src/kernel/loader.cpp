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
    kout << "============\n";
    kout << "renew-DOS v0.2\n";
    kout << "============\n";
    kout << "FROM: Riel Faimel\n";

    // init work running on CPU0
    ClusterDeviceManager cdm{};
    PeripheralDeviceSpace pds{};
    KernelModule kernelmodule;
    while(1);
    pds["kernel module"] = &kernelmodule;
    TaskManager taskmgr;
    kout << "[INFO] start INIT.COM\n";
    ASMLabelMark(LoaderMain_StartB_INIT_EXE)
    auto err = taskmgr.exec("B:\\INIT.COM", 0, nullptr);
    switch (err) {
    case FexecExceptionCode::NoException: {
        // done
    } break;
    case FexecExceptionCode::OpenFileFailed: {
        kout << "[Exception] \"B:\\INIT.COM\" not found\n";
    } break;
    case FexecExceptionCode::DLSFault: {
        kout << "[Fault] File System Fault\n";
    } break;
    default: {
        kout << "[Fault] Bad Fault\n";
    } break;
    }
    _Sexit(); // init thread sleep
}
