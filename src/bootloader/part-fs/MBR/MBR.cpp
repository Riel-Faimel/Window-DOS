#include "_MBR.hpp"
#include <drv/screen/screen_srv.hpp>

/**
 * MODEL:
 * construct object from disk.
 * if there 0xAA55 then read MBR and synch
 * if no 0xAA55 skip and set no MBR
 * 
 * for part in MBR readed system ID
 * if 0x00 then no part, so skip
 * if 0xFF then RAW, there is part but no file system
 * 
 * else others just file system ID
 * 
 * ======
 * 
 * no MBR can make_MBR:
 * clear part table and set 0xAA55
 * 
 * no part can make_part:
 * search 0x00(no part). when found set system ID and write back
 * then 0x00 turn to 0xFF
 * 
 * no file system can init_part:
 * call by file system, set system ID from file system.
 */

DISK_PART::DISK_PART(LogicalDisk *disk_choose):
disk(disk_choose),
has_MBR(false){
    MBR mbr;
    asm volatile (
        ".globl DEBUG_4\n"
        "DEBUG_4:"
    );
    disk->read(mbr.buf, 0, 0, 512);
    if(mbr.sign == (unsigned short)0xAA55){
        for(unsigned char i = 0;i < 4;i++){
            if(mbr.part[i].system_id){
                part_info[i].start = mbr.part[i].start_LBA_high << 16 | mbr.part[i].start_LBA_low;
                part_info[i].part_size = mbr.part[i].sector_count_high << 16 | mbr.part[i].sector_count_low;
                part_info[i].sectors_per_block = 1;
                part_info[i].system_ID = static_cast<System_ID>(mbr.part[i].system_id);
            }
        }
        has_MBR = true;
    }
}

void DISK_PART::make_MBR(){
    if(has_MBR)return;
    MBR mbr;
    disk->read(mbr.buf, 0, 0, 512);
    for(int i = 0;i < 4;i++){
        mbr.part[i] = {};
    }
    mbr.sign = 0xAA55;
    disk->write(mbr.buf, 0, 0, 512);
}

unsigned int DISK_PART::make_part(bool can_boot, unsigned int from, unsigned int to){
    if(!has_MBR)return 0xFFFFFFFF;
    MBR mbr;
    disk->read(mbr.buf, 0, 0, 512);
    unsigned i;
    for(i = 0;i < 4;i++){
        if(mbr.part[i].system_id == 0){ //only look for 0x00
            unsigned char boot_flag = 0x00;
            if(can_boot)boot_flag = 0x80;
            mbr.part[i] = {
                boot_flag, 0x20, 0x21, 0, 0xDA, 
                0xFE, 0xFF, 0xFF, (unsigned short)(from & 0xFFFF), 
                (unsigned short)((from >> 16) & 0xFFFF), (unsigned short)((to - from) & 0xFFFF), 
                (unsigned short)(((to - from) >> 16) & 0xFFFF), 
            };
            disk->write(mbr.buf, 0, 0, 512);
            part_info[i] = {from, to - from, 1, System_ID::RAW};
            return i;
        }
    }
    return 0xFFFFFFFF;
}

void DISK_PART::init_part(System_ID system_id, unsigned int part_id){
    if(part_id >= 4 || part_info[part_id].system_ID != System_ID::RAW)return; //only deal 0xFF
    part_info[part_id].system_ID = system_id;
    MBR mbr;
    disk->read(mbr.buf, 0, 0, 512);
    mbr.part[part_id].system_id = static_cast<unsigned char>(system_id);
    disk->write(mbr.buf, 0, 0, 512);
}

unsigned char DISK_PART::get_system_id(unsigned int part_id){
    if(part_id >= 4)return 0x00;
    return static_cast<unsigned char>(part_info[part_id].system_ID);
}

void DISK_PART::read(unsigned short *buf, unsigned int part_id, unsigned int start_block, unsigned int blocks_read){
    unsigned sec_per_blk = part_info[part_id].sectors_per_block;
    unsigned lba = part_info[part_id].start + 
        start_block * sec_per_blk;
    unsigned sec_num = blocks_read * sec_per_blk;
    disk->read(buf, lba, 0, sec_num * 512);
}

void DISK_PART::write(unsigned short *buf, unsigned int part_id, unsigned int start_block, unsigned int blocks_write){
    unsigned sec_per_blk = part_info[part_id].sectors_per_block;
    unsigned lba = part_info[part_id].start + 
        start_block * sec_per_blk;
    unsigned sec_num = blocks_write * sec_per_blk;
    disk->write(buf, lba, 0, sec_num * 512);
}

void DISK_PART::set_block(unsigned int part_id, unsigned int sectors_per_block){
    part_info[part_id].sectors_per_block = sectors_per_block;
}