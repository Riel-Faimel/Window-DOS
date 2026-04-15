#include "las.hpp"
#include "part-fs.hpp"
#include <drv/PCI/pci.hpp>
#include <lib/cppstdlib/string>
#include <drv/disk/IDE/ide.hpp>
#include <drv/screen/screen_srv.hpp>

#include <CenterShell/cs.hpp>


inline void __dir(String str){
    if(str == "dir"){
        screen->print("Drive");screen->print("");screen->print("L");
        return;
    };
}

inline void mkfs_(String str){
    if(str.substr(0, 11) == "mkfs.fat16 "){
        auto disk_part_uid = str.extract_int("--uid 0x");
        linear_address_space->mkfs_FAT16(disk_part_uid);
        print_char('\n');
    }
    else {
        screen->print("Invaild params\n");
    };
}

inline void alloc_driver_letter(String str){
    if(str.substr(0, 4) == "set "){
        auto disk_part_uid = str.extract_int("--id 0x");
        if(disk_part_uid == 0xFFFFFFFF)return;
        String letter = str.extract_param("/");
        linear_address_space->set_letter(disk_part_uid, letter);
    }
    else {
        screen->print("Invaild params\n");
    }
}