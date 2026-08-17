#include "loader.hpp"

volatile descrptor::IDTEntry _idt_space[256];
volatile descrptor::Entry _gdt_space[256];
registry_editor reg_edit;

__attribute__((optimize("O0")))
void LoaderMain(){
    CPU cpu0{_idt_space, _gdt_space, 256};

    //===INFO===
    cout << "============\n";
    cout << "Window-DOS v0.1\n";
    cout << "============\n";
    cout << "FROM: Riel Faimel\n\n";
    cout << "[INFO] boot device ID: " << boot_infomation->boot_device << '\n';

    asm volatile ("sti");

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
