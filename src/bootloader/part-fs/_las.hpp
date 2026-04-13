#include "las.hpp"
#include "part-fs.hpp"
#define __independent_lib_Using_template_container
#include <TL/idlib/idlib>
#include <drv/PCI/pci.hpp>
#include <lib/cppstdlib/string.hpp>
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
        auto part_id = str.extract_int("-p 0x");
        linear_address_space->mkfs_FAT16(linear_address_space->dealing_id, part_id);
        print_char('\n');
    }
    else {
        screen->print("Invaild params\n");
    };
}

inline void alloc_driver_letter(String str){
    if(str.substr(0, 4) == "set "){
        auto disk_id = str.extract_int("--id 0x");
        String letter = str.extract_param("/");
        linear_address_space->set_letter(disk_id, letter);
    }
    else {
        screen->print("Invaild params\n");
    }
}