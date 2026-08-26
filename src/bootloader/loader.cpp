#include "loader.hpp"
registry_editor reg_edit;
extern MemoryManager *global_heap;

__attribute__((optimize("O0")))
void LoaderMain(){
    MemoryManager init_global_heap{false};
    global_heap = &init_global_heap;
    CPU cpu0{};

    //===INFO===
    cout << "============\n";
    cout << "Window-DOS v0.2\n";
    cout << "============\n";
    cout << "FROM: Riel Faimel\n\n";
    cout << "[INFO] boot device ID: " << boot_infomation->boot_device << '\n';


    asm volatile ("sti");

    // init work running on CPU0
    StorageSubSystem sss{};
    PeripheralDeviceSpace pds{};
    pds.probe();
    pds.set_driver();
    
    TaskManager taskmgr;

    taskmgr.exec("B:\\INIT.EXE");
    cpu0.preempt();
    _Sexit(); // init thread sleep
}
