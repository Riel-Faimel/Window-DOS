#include "loader.hpp"

mm reserved_space_for_mm;
BootINFO *boot_infomation = reinterpret_cast<BootINFO *>(0x7C00 + 320);
_Screen *screen;
volatile IDT::IDT_item _IDT[256];
volatile GDT::GDTEntry _gdt_space[8192];

Window<32> win;

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
/*
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
*/
IDT idt(_IDT);

__attribute__((optimize("O0")))
void LoaderMain(){
    asm volatile ("cli");

    /**
     * TODO: (Done)
     * take care of the address, if it write the code section?
     * 
     * TODO:
     * take care if this allloccater full
     */
    new (&reserved_space_for_mm) mm(4096, 1024, (void *)0x100000, __boot_loader_end);
    registry_editor reg_edit;
    screen = init_screen();

    //===INFO===
    screen->print("============\nWindow-DOS v0.1\n============\r\nFROM: Riel Faimel\r\n\r\n");

    screen->print("[INFO] protect mode\n[INFO] boot device ID: ");

    print_hex(boot_infomation->boot_device);
    screen->print("\n");
    idt.set_PIC();
    screen->print("[INFO] PIC set done\n");

    asm volatile ("sti");

    PDS pds{};
    StorageSubSystem sss{};
    
    PCI_space PCI_device_spaceP{false};
    PCI_device_spaceP.set_device_driver();
    //while(1){asm volatile ("hlt");}
    //DOScall disk_operating_system_system_call{idt};
    GDT gdt{_gdt_space, 8192, idt};
    //CenterShell cs;
    #pragma pack(push, 1)
    struct {
        _WIN h;
        unsigned char buf[512];
    } win;
    #pragma pack(pop)
    if (linear_address_space->open(win.h, "B:\\INIT") == (unsigned)-1) \
    { kprint("    Not found initializer!\n"); } \
    else kprint("Found INIT\n");
    
    linear_address_space->read(win.h, 0, 512);
    kprint("    Kernel initialized done!\r\n");
    //for (unsigned i = 0;i < 512;i++) {\
        auto ch = win.buf[i];\
        print_hex(ch, false);print_char(' ');\
    }
}
