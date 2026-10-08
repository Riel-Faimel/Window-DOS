#include "loader.hpp"
extern MemoryManager *global_heap;

extern void f(int);

__attribute__((optimize("O0"), noreturn))
void LoaderMain(){
    MemoryManager init_global_heap{false};
    global_heap = &init_global_heap;
    CPU cpu0{};
    cpu0.preempt();
    asm volatile ("sti");

    //===INFO===
    cout << "============\n";
    cout << "renew-DOS v0.2\n";
    cout << "============\n";
    cout << "FROM: Riel Faimel\n\n";

    // init work running on CPU0
    KernelModule kernelmodule;
    ClusterDeviceManager cdm{};
    PeripheralDeviceSpace pds{};
    pds.probe();
    TaskManager taskmgr;
    cout << "[INFO] start INIT.COM\n";
    ASMLabelMark(LoaderMain_StartB_INIT_EXE)
    auto err = taskmgr.exec("B:\\INIT.COM", 0, nullptr);
    switch (err) {
    case FexecExceptionCode::NoException: {
        // done
    } break;
    case FexecExceptionCode::OpenFileFailed: {
        cout << "[Exception] \"B:\\INIT.COM\" not found\n";
    } break;
    case FexecExceptionCode::DLSFault: {
        cout << "[Fault] File System Fault\n";
    } break;
    default: {
        cout << "[Fault] Bad Fault\n";
    } break;
    }
    _Sexit(); // init thread sleep
}
