#include "loader.hpp"

BootINFO *boot_infomation = reinterpret_cast<BootINFO *>(0x7C00 + 320);
_Screen *screen;
volatile IDT::IDT_item _IDT[256];
volatile GDT::GDTEntry _gdt_space[8192];
IDT idt(_IDT);

_Screen *init_screen(){
    if(boot_infomation->screen_mode){
        //return new VBE;
        _Screen *re = new VGA_text_mode;
        re->clear();
        re->print("[INFO] VBE mode\r\n");
        return re;
    }else{
        if(boot_infomation->graphic_mode){
            //return new VGA_graphic_mode;
            _Screen *re = new VGA_text_mode;
            re->clear();
            re->print("[INFO] VGA graphic mode\r\n");
            return re;
        }else{
            _Screen *re = new VGA_text_mode;
            re->clear();
            re->print("[INFO] VGA text mode\r\n");
            return re;
        }
    };
}


__attribute__((optimize("O0")))
void LoaderMain(){
    /**
     * TODO: (Done)
     * take care of the address, if it write the code section?
     * 
     * TODO:
     * take care if this allloccater full
     */
    mm(4096, 4096, (void *)0x100000, __boot_loader_end);
    registry_editor reg_edit;
    screen = init_screen();

    //===INFO===
    screen->print("============\nWindow-DOS v0.1\n============\r\nFROM: Riel Faimel\r\n\r\n");
    screen->print("[INFO] protect mode\n[INFO] boot device ID: ");
    print_hex(boot_infomation->boot_device);print_char('\n');

    idt.set_PIC();
    screen->print("[INFO] PIC set done\n");

    asm volatile ("sti");
    GDT gdt{_gdt_space, 8192, idt};

    StorageSubSystem sss{};
    PDS pds{};
    pds.probe();
    pds.set_driver();
    
    TaskManager taskmgr;

    taskmgr.exec("B:\\INIT.EXE");
}
