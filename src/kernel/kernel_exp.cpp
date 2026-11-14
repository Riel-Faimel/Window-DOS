#include "kernel.hpp"
extern MemoryManager *global_heap;

extern void f(int);
iostream kout;

__attribute__((optimize("O0"), noreturn))
void LoaderMain(){
    MemoryManager init_global_heap{false};
    global_heap = &init_global_heap;
    CurruentArchitecture cpu0{}; // init first CPU
    cpu0.preempt();
    interrupt_distributor.append_cpu(cpu0);
    asm volatile ("sti");

    //===INFO===
    kout << "==============\n"
         << "renew-DOS v0.2\n"
         << "==============\n"
         << "By Riel Faimel\n";

    // init work running on CPU0
    try {
        class Kernel {
        public:
            WindowManager wm{};
            ClusterDeviceManager cdm{};
            PeripheralDeviceSpace pds{};
            TaskManager taskmgr;
            KernelModule kernelmodule;
        } renew_DOS;
        
        kout << "[BOOT] start INIT.COM\n";
        ASMLabelMark(LoaderMain_StartB_INIT_EXE)
        auto err = renew_DOS.taskmgr.exec("B:\\INIT.COM", 0, nullptr);
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
        BasicShell bs;
        bs.main();
    } catch (...) {
        kout << "[BOOT] <exception>\n";
        _Sexit();
    }
}
