#include "ide.hpp"
#include <drv/screen/screen_srv.hpp>
#include <IDT_L/idt_.hpp>
#define _BITS_32
#include <global/type.hpp>
#include "ide_controller_init.hpp"
#include <mm/mm.hpp>
#define __independent_lib_Using_reference_part
#include <TL/idlib/idlib>

void init_IDE_controller(void* *ptr, size_t *size){
    IDE_DISK *re = new IDE_DISK[4];
    new IDE_Channal[2]{
        {re[0], re[1], IDE_Channal::Channal::Master_Channel, idt}, 
        {re[2], re[3], IDE_Channal::Channal::Slave_Channel, idt}, 
    };
    u8 exist_disk = 0;
    u8 j = 0;
    for(u8 i = 0;i < 4;i++)if(re[i].exist)exist_disk++;
    IDE_DISK *re_;
    if(exist_disk > 0) re_ = new IDE_DISK[exist_disk];
    else {
        screen->print("[INFO] No disk found\r\n");
        *ptr = nullptr;
        *size = 0;
        return;
    }
    for(u8 i = 0;i < 4;i++){
        if(re[i].exist){
            re_[j] = rtl::move(re[i]);
            j++;
        }
    }
    delete[] re;
    *ptr = re_;
    *size = exist_disk;
    screen->print("[INFO] IDE Disk: ");
    print_hex(exist_disk);
    screen->print("\r\n");
}