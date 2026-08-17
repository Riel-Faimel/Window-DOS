#include "loader.hpp"
registry_editor reg_edit;

__attribute__((optimize("O0")))
void LoaderMain(){
    CPU cpu0{};

    //===INFO===
    cout << "============\n";
    cout << "Window-DOS v0.2\n";
    cout << "============\n";
    cout << "FROM: Riel Faimel\n\n";
    cout << "[INFO] boot device ID: " << boot_infomation->boot_device << '\n';

    asm volatile ("sti");
    while(1);

    // init work running on CPU0
    StorageSubSystem sss{};
    PeripheralDeviceSpace pds{};
    pds.probe();
    pds.set_driver();
    
    TaskManager taskmgr;

    taskmgr.exec("B:\\INIT.EXE");
    /**
     * shouldn't fall through to here
     */

    cout << "Unexcept Exit!\n";
}
