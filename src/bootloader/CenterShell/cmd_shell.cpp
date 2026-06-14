#include "_cmd_shell.hpp"
#include <part-fs/MBR/MBR.hpp>
#include <drv/disk/IDE/ide.hpp>
#include <lib/hardlib/x86/port.h>
#include <part-fs/las.hpp>

extern "C" __attribute__((cdecl)) void do_int_with_params(
    u8 vector, u32 eax, u32 ebx, u32 ecx, u32 edx, u32 esi, u32 edi
);

constexpr unsigned line_nums = 16;
constexpr unsigned lines = 16;
void cmd_see(String args){
    static String last_param{};
    if(args.substr(0, 2) == "0x"){
        unsigned addr = args.substr(2).to_int();
        unsigned char *ptr = reinterpret_cast<unsigned char *>(addr);
        for(unsigned i = 0; i < lines; i++){
            print_hex(addr + i * line_nums);
            screen->print(": ");
            for(unsigned j = 0; j < line_nums; j++){
                print_hex(ptr[i * line_nums + j], false);
                print_char(' ');
            }
            print_char(' ');
            for(unsigned j = 0; j < line_nums; j++){
                unsigned char ch = ptr[i * line_nums + j];
                if(ch >= 32 && ch <= 126){
                    print_char(ch);
                }else{
                    print_char('.');
                }
            }
            print_char('\n');
        }
    }
    else if (args.substr(0, 1) == "$"){
        String var_name = args.substr(1);
        if(var_name == "esp"){
            unsigned esp;
            asm volatile("mov %%esp, %0" : "=r"(esp));
            screen->print("ESP = 0x");print_hex(esp);screen->print("\r\n");
        }
        else if(var_name == "ebp"){
            unsigned ebp;
            asm volatile("mov %%ebp, %0" : "=r"(ebp));
            screen->print("EBP = 0x");print_hex(ebp);screen->print("\r\n");
        }
        else if(var_name == "cr0"){
            unsigned cr0;
            asm volatile("mov %%cr0, %0" : "=r"(cr0));
            screen->print("CR0 = 0x");print_hex(cr0);screen->print("\r\n");
        }
        else if(var_name == "cr3"){
            unsigned cr3;
            asm volatile("mov %%cr3, %0" : "=r"(cr3));
            screen->print("CR3 = 0x");print_hex(cr3);screen->print("\r\n");
        }
        else if(var_name == "cr4"){
            unsigned cr4;
            asm volatile("mov %%cr4, %0" : "=r"(cr4));
            screen->print("CR4 = 0x");print_hex(cr4);screen->print("\r\n");
        }
        else{
            screen->print("\r\ninvalid variable name.\r\n\nhelp: see $esp|$ebp|$cr0|$cr3|$cr4\r\n");
        }
    }
    else if(args.substr(0, 3) == ":0x"){
        String port_str = args.substr(3);
        u16 port = port_str.to_int();
        if (args.find(" -d") != -1){
            u32 data = inb(port);
            screen->print("Port ");print_hex(port);screen->print(" = ");print_hex(data);screen->print("\r\n");
        }
        else if(args.find(" -w") != -1){
            u16 data = inb(port);
            screen->print("Port ");print_hex(port);screen->print(" = ");print_hex(data);screen->print("\r\n");
        } 
        else if (args.find(" -b") != -1){
            u8 data = inb(port);
            screen->print("Port ");print_hex(port);screen->print(" = ");print_hex(data);screen->print("\r\n");
        } 
        else {
            u8 data = inb(port);
            screen->print("Port ");print_hex(port);screen->print(" = ");print_hex(data);screen->print("\r\n");
        }
    }
    else if(args == "help"){
        screen->print("\r=== SEE COMMAND HELP ===\r\n");
        screen->print("see 0x... - view memory content from the address\r\n");
        screen->print("see $... - view register value (esp, ebp, cr0, cr3, cr4)\r\n");
        screen->print("see :0x... - view port value\r\n");
        screen->print("\"see\" only for re-see\r\n");
    }
    else if (args == ""){
        if(last_param.empty()){
            screen->print("\r\nno last parameter.\r\n");
            return;
        }
        cmd_see(last_param);
        return;
    }
    else {
        screen->print("\r\ninvalid parameter.\r\n\nhelp: see help\r\n");
        return;
    }
    last_param = args;
}

void cmd_disk(String args){
    static u8 device_id = 0xFF;
    if(args == "info"){
        for(unsigned i = 0;i < PCI_device_numbers;i++){
            if(PCI_device_config_pointer[i].Class_code[2] == 0x01){
                if(PCI_device_config_pointer[i].Class_code[1] == 0x01){
                    screen->print("IDE controller\nDisk Number: ");
                    print_hex(PCI_device_config_pointer[i].size);screen->print("\n");
                }
                else if (PCI_device_config_pointer[i].Class_code[1] == 0x02)screen->print("SATA controller\n");
            }
        };
    }
    else if (args.substr(0, 6) == "choose"){
        int id = args.extract_int("0x");
        if(id > 4 || id < 0){
            screen->print("\r\ninvalid device id\r\n");
            return;
        }
        device_id = id;
        screen->print("\r\nDevice chosen: ");
        print_hex(device_id);screen->print("\r\n");
    }
    else if (args.substr(0, 4) == "read"){
        for(unsigned i = 0;i < PCI_device_numbers;i++){
            if(PCI_device_config_pointer[i].Class_code[2] == 0x01){
                if(PCI_device_config_pointer[i].Class_code[1] == 0x01){
                    unsigned disk_num;
                    if(device_id == 0xFF){
                        disk_num = args.extract_int("-d 0x");
                    }
                    else {
                        disk_num = device_id;
                    };
                    int LBA = args.extract_int("-l 0x");
                    int count = args.extract_int("-n 0x");
                    void *addr = reinterpret_cast<void*>(args.extract_int("-a 0x"));
                    IDE_DISK *disks = static_cast<IDE_DISK*>(PCI_device_config_pointer[i].dev_drv);
                    if(disk_num >= PCI_device_config_pointer[i].size){
                        screen->print("\r\ninvalid disk number\r\n");
                        return;
                    }
                    if(!disks[disk_num].exist){
                        screen->print("\r\ndisk not exist\r\n");
                        return;
                    }
                    disks[disk_num].read(static_cast<unsigned short*>(addr), LBA, 0, count * 512);
                    screen->print("\r\nRead Done!\r\n");
                    return;
                }
                else if (PCI_device_config_pointer[i].Class_code[1] == 0x02){
                    if(device_id == 0xFF){
                        screen->print("SATA read not implemented yet\r\n");
                        return;
                    }
                    else {
                        screen->print("SATA read not implemented yet\r\n");
                        return;
                    };
                }
            }
        };
    }
    else if (args.substr(0, 5) == "write"){
        for(unsigned i = 0;i < PCI_device_numbers;i++){
            if(PCI_device_config_pointer[i].Class_code[2] == 0x01){
                if(PCI_device_config_pointer[i].Class_code[1] == 0x01){
                    unsigned disk_num;
                    if(device_id == 0xFF){
                        disk_num = args.extract_int("-d 0x");
                    }
                    else {
                        disk_num = device_id;
                    };
                    int LBA = args.extract_int("-l 0x");
                    int count = args.extract_int("-n 0x");
                    void *addr = reinterpret_cast<void*>(args.extract_int("-a 0x"));
                    IDE_DISK *disks = static_cast<IDE_DISK*>(PCI_device_config_pointer[i].dev_drv);
                    if(disk_num >= PCI_device_config_pointer[i].size){
                        screen->print("\r\ninvalid disk number\r\n");
                        return;
                    }
                    disks[disk_num].write(static_cast<unsigned short*>(addr), LBA, 0, count * 512);
                    screen->print("\r\nWrite Done!\r\n");
                    return;
                }
                else if (PCI_device_config_pointer[i].Class_code[1] == 0x02){
                    screen->print("SATA write not implemented yet\r\n");
                    return;
                }
            }
        };
    }
    else if (args.substr(0, 5) == "part "){
        args = args.substr(5);
        if (args.substr(0, 4) == "set "){
            unsigned from = args.extract_int("--0x");
            unsigned to = args.extract_int(":0x");
            u8 disk_id = args.extract_int("-d 0x");
            String le = args.extract_param("/");
            if(disk_id == 0xFF){
                if(device_id != 0xFF)disk_id = device_id;
                else{
                    screen->print("[ERROR] Which disk?\n");
                    return;
                }
            }
            print_hex(linear_address_space->set_part(disk_id, from, to, le));
            print_char('\n');
        }
        else if (args == "show"){
            screen->print("All driver letters: \n ");
            linear_address_space->show_driver();
            print_char('\n');
        }
    }
    else if (args == "help"){
        screen->print("\r=== DISK HELP ===\r\n");
        screen->print("disk info - show disk information\r\n");
        screen->print("disk choose 0x... - choose disk by id (cover -d param)\r\n");
        screen->print("disk read -d 0x... -l 0x... -n 0x... -a 0x... - LBA read\r\n");
        screen->print("disk write -d 0x... -l 0x... -n 0x... -a 0x... - LBA write\r\n");
        screen->print("disk part [show | set --0x...:0x...] - part operations\n");
    }
    else screen->print("\r\ninvalid parameter.\r\n\ntry: disk help\n");
}

void cmd_say(String args){
    if(args.substr(0, 7) == "memory " || args.substr(0, 2) == "M "){
        void *addr = reinterpret_cast<void*>(args.extract_int("--0x"));
        u32 data = args.extract_int(":0x");
        *reinterpret_cast<u8*>(addr) = data & 0xFF;
        while(data >> 8){
            data >>= 8;
            addr+=1;
            *reinterpret_cast<u8*>(addr) = data & 0xFF;
        }
        screen->print("\r\nWrite Done!\r\n");
    }
    else if(args.substr(0, 5) == "port " || args.substr(0, 2) == "P "){
        u16 port = args.extract_int("--0x");
        u32 data = args.extract_int(":0x");
        if(data <= 0xFF) outb(static_cast<u8>(data), port);
        else if(data <= 0xFFFF) outb(static_cast<u16>(data), port);
        else outb(data, port);
        screen->print("\r\nWrite Done!\r\n");
    }
    else if (args.substr(0, 4) == "reg " || args.substr(0, 2) == "R "){
        String r = args.extract_param("--$");
        u32 val = args.extract_int(":0x");
        if(r == "eax"){
            asm volatile (
                "mov %0, %%eax\n"
                : : "r"(val) : "eax"
            );
            screen->print("\nWrite Done!\n");
            return;
        }
        else if(r == "ebx"){
            asm volatile (
                "mov %0, %%ebx\n"
                : : "r"(val) : "ebx"
            );
            screen->print("\nWrite Done!\n");
            return;
        }
        else if(r == "ecx"){
            asm volatile (
                "mov %0, %%ecx\n"
                : : "r"(val) : "ecx"
            );
            screen->print("\nWrite Done!\n");
            return;
        }
        else if(r == "edx"){
            asm volatile (
                "mov %0, %%edx\n"
                : : "r"(val) : "edx"
            );
            screen->print("\nWrite Done!\n");
            return;
        }
        else if(r == "esi"){
            asm volatile (
                "mov %0, %%esi\n"
                : : "r"(val) : "esi"
            );
            screen->print("\nWrite Done!\n");
            return;
        }
        else if(r == "edi"){
            asm volatile (
                "mov %0, %%edi\n"
                : : "r"(val) : "edi"
            );
            screen->print("\nWrite Done!\n");
            return;
        }
        /**
         * TODO:(done by Copilot)
         * for Copilot
         * we need eax, ebx, ecx, edx, esp, eip, esi, edi
         * eax is up there, do as i do
         */
    }
    else if(args == "help"){
        screen->print("\r=== SAY COMMAND HELP ===\r\n");
        screen->print("say memory | M --0x...:0x... - write data to memory\r\n");
        screen->print("say port --0x...:0x... - write data to port\r\n");
        screen->print("say reg | R --0x...:0x... - write data to register\r\n");
    }
    else screen->print("\r\ninvalid parameter.\r\n\nhelp: say memory --0x... :0x...\r\n");
}

void cmd_int(String args){
    if(args == "help"){
        screen->print("\r=== INT COMMAND HELP ===\r\n");
        screen->print("int 0x.. - trigger software interrupt and jump to IDT entry\r\n");
        screen->print("Also -a 0x... for param in eax, and so on(-b, -c ...)\r\n");
        return;
    }
    else if(args.substr(0, 2) == "0x"){
        u8 vector = static_cast<u8>(args.extract_int("0x"));
        u32 eax_val = args.extract_int("-a 0x");
        u32 ebx_val = args.extract_int("-b 0x");
        u32 ecx_val = args.extract_int("-c 0x");
        u32 edx_val = args.extract_int("-d 0x");
        u32 esi_val = args.extract_int("-s 0x");
        u32 edi_val = args.extract_int("-i 0x");
        /**
         * THANKS:
         * Done by Copilot
         */

        do_int_with_params(vector, eax_val, ebx_val, ecx_val, edx_val, esi_val, edi_val);
        //screen->print("[INFO] come back");
        return;
    }
    screen->print("\r\ninvalid parameter.\r\n\nhelp: int help\r\n");
}

void cmd_jmp(String args) {
    if(args == "help"){
        screen->print("\r=== JMP COMMAND HELP ===\r\n");
        screen->print("jmp 0xfrom:0xto - execute code in the range [from, to)\r\n");
        return;
    }
    else if(args.substr(0, 2) == "0x"){
        int colon_pos = args.find(":0x");
        if(colon_pos == -1) {
            //screen->print("Usage: jmp 0xfrom:0xto\n");
            return;
        }
        
        unsigned from = args.substr(2, colon_pos - 2).to_int();
        unsigned to = args.substr(colon_pos + 3).to_int();
        
        if(from >= to) {
            screen->print("Invalid range: from must be less than to\n");
            return;
        }
        
        screen->print("Executing code from 0x");
        print_hex(from);
        screen->print(" to 0x");
        print_hex(to);
        screen->print("\n");

        //unsigned len = to - from;
        
        unsigned char* end_ptr = reinterpret_cast<unsigned char*>(to);
        *end_ptr = 0xC3;  // ret
        
        asm volatile(
            "pusha\n"
            "call *%0\n"
            "popa\n"
            : : "r"(from) : "memory"
        );
    }
    else screen->print("\r\ninvalid parameter.\r\n\nhelp: jmp 0xfrom:0xto\r\n");
}

void cmd_shell(String cmd_line){
    if(cmd_line.empty())return;

    if(cmd_line == "help" || cmd_line == "H") {
        //screen->print("\r=== HELP MESSAGE ===\r\nKernel: Window-DOS - version 0.1\r\nActiving\r\n====================\r\n");
        screen->print("Try: see | disk | echo | say | jmp | int \r\n");
    }
    else if(cmd_line.substr(0, 4) == "see " || cmd_line == "see") cmd_see(cmd_line.substr(4));
    else if(cmd_line.substr(0, 5) == "disk ") cmd_disk(cmd_line.substr(5));
    else if (cmd_line == "clear") screen->clear();
    else if (cmd_line.substr(0, 5) == "echo ") {
        screen->print(cmd_line.substr(5).c_str());
        screen->print("\r\n");
    }
    else if (cmd_line.substr(0, 4) == "say ") {
        cmd_say(cmd_line.substr(4));
    }
    else if (cmd_line.substr(0, 4) == "int ") {
        cmd_int(cmd_line.substr(4));
    }
    else if (cmd_line.substr(0, 4) == "jmp ") {
        cmd_jmp(cmd_line.substr(4));
    }
    else {
        screen->print("\nCommand not found:");
        screen->print(cmd_line);
        print_char('\n');
    }
}