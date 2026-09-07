#include "loader.hpp"
extern MemoryManager *global_heap;

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
    StorageSubSystem sss{};
    cout << "[INFO] DLS is ok\n";
    PeripheralDeviceSpace pds{};
    pds.probe();
    pds.set_driver();
    TaskManager taskmgr;
    cout << "[INFO] start INIT.COM\n";
    auto err = taskmgr.exec("B:\\INIT.COM", 0, nullptr);
    if (err != 0) switch (err) {
    case (unsigned)-1:{
        cout << "[FAILED] \"B:\\INIT.COM\" not found\n";
    }
    default: {}
    }
    _Sexit(); // init thread sleep
}
