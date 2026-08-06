#include "loader.hpp"

volatile descrptor::IDTEntry _IDT[256];
volatile descrptor::Entry _gdt_space[8192];
IDT idt(_IDT);
registry_editor reg_edit;

__attribute__((optimize("O0")))
void LoaderMain(){
    /**
     * TODO: (Done)
     * take care of the address, if it write the code section?
     * 
     * TODO:
     * take care if this allloccater full
     */
    GDT gdt{_gdt_space, 8192, idt};
    VGA_text_mode VGA_screen;
    MemoryManager mm{};

    //===INFO===
    screen->print("============\nWindow-DOS v0.1\n============\r\nFROM: Riel Faimel\r\n\r\n");
    screen->print("[INFO] boot device ID: ");
    print_hex(boot_infomation->boot_device);print_char('\n');

    idt.set_PIC();
    screen->print("[INFO] PIC set done\n");

    asm volatile ("sti");

    struct m {u32 v[13];};
    m *a = new m;
    print_hex((u32)a);
    while(1);
    StorageSubSystem sss{};
    PeripheralDeviceSpace pds{};
    pds.probe();
    pds.set_driver();
    
    TaskManager taskmgr;

    taskmgr.exec("B:\\INIT.EXE");
    /**
     * shouldn't fall through to here
     */

    kprint("Unexcept Exit!\n");
}
