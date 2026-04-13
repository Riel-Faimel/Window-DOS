#include "loader.hpp"

mm reserved_space_for_mm;
BootINFO *boot_infomation = reinterpret_cast<BootINFO *>(0x7C00 + 320);
_Screen *screen;
volatile IDT::IDT_item _IDT[256];

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

void print_esp() {
    void *p;
    screen->print("esp: ");
    asm volatile (
        "mov %%esp, %0"
        : "=r"(p)
    );
    print_hex(reinterpret_cast<unsigned>(p));
    screen->print("\r\n");
}

void print_ss() {
    void *p;
    screen->print("ss: ");
    asm volatile (
        "mov %%ss, %0"
        : "=r"(p)
    );
    print_hex(reinterpret_cast<unsigned>(p));
    screen->print("\r\n");
}

IDT idt(_IDT);

void LoaderMain(){
    asm volatile ("cli");

    /**
     * TODO:
     * take care of the address, if it write the code section?
     */
    new (&reserved_space_for_mm) mm(4096, 256, (void *)0x100000, __boot_loader_end);
    registry_editor reg_edit;
    screen = init_screen();

    //===INFO===
    screen->print("============\nWindow-DOS v0.1\n============\r\nFROM: Riel Faimel\r\n\r\n");

    screen->print("[INFO] protect mode\r\n[INFO] boot device ID: ");

    print_hex(boot_infomation->boot_device);
    screen->print("\r\n");

    qps("start init PIC\r\n");
    idt.set_PIC();
    screen->print("[INFO] PIC set done\r\n");

    asm volatile ("sti");

    PCI_space PCI_device_spaceP{false};
    PCI_device_spaceP.set_device_driver();
/*
    unsigned i;
    PCI_device_config *cfg;
    for(i = 0;i < PCI_device_spaceP.config.get_size();i++){
        cfg = &PCI_device_spaceP.config[i];
        if(cfg->Class_code[2] == 0x01 && cfg->Class_code[1] == 0x01){
            break;
        }
    }
    DISK_PART p[2] = {{&static_cast<IDE_DISK *>(cfg->dev_drv)[0]}, {&static_cast<IDE_DISK *>(cfg->dev_drv)[1]}};
    FAT16 fs[2] = {{&p[0], 0, true}, {&p[1], 0}};
*/
    LAS las{};
    DOScall disk_operating_system_system_call{idt};
    CenterShell cs;
    print_ss();
    //while (true){asm volatile ("hlt");}
}
