#include "kernel.hpp"
#include <syscall/thread>
#include <drv/screen/screen_srv.hpp>
#ifdef COMDebug
#include <drv/com/com.hpp>
#endif

extern void f(int);

// COMDebug：把控制台输出改到 COM1，便于没有屏幕的场合取内核日志。
#ifdef COMDebug
ostream kout{&comout};
#else
ostream kout{&vgatextout};
#endif

__attribute__((optimize("O0"), noreturn))
void LoaderMain(){
    MemoryManager global_heap;
    TaskManager taskmgr;

    // init first CPU
    CurruentArchitecture cpu0{};
    cpu0.preempt();
    cpu0.mem_protect();
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
            KernelObjectManager kom{};
            KernelModule kernelmodule;
        } renew_DOS;
        kout << "[BOOT] start INIT.COM\n";
        ASMLabelMark(LoaderMain_StartB_INIT_EXE)
        auto err = taskmgr.exec("B:\\INIT.COM", 0, nullptr);
        switch (err) {
        case FexecExceptionCode::NoException: {
            _Sexit(); // init thread sleep
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
