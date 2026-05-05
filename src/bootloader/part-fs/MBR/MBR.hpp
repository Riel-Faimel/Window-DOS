#ifndef __bootloader_part_fs_MBR_MBR_hpp
#define __bootloader_part_fs_MBR_MBR_hpp
#include <drv/disk/disk.hpp>

class DISK_PART{
public:
#pragma pack(push, 1)
    typedef struct __attribute__((packed)){
        unsigned char boot_flag;
        unsigned char start_head;
        unsigned char start_sector;
        unsigned char start_cylinder;
        unsigned char system_id;
        unsigned char end_head;
        unsigned char end_sector;
        unsigned char end_cylinder;
        unsigned short start_LBA_low;
        unsigned short start_LBA_high;
        unsigned short sector_count_low;
        unsigned short sector_count_high;
    }PE;
    union MBR{
        unsigned short buf[256];
        struct {
            unsigned char boot[446];
            PE part[4];
            unsigned short sign;
        }__attribute__((packed));
    };
#pragma pack(pop)

    enum class System_ID : unsigned char {
        NULL = 0x00,
        FAT12 = 0x01,
        FAT16 = 0x04,
        FAT16_ = 0x06,
        FAT32 = 0x0B,
        FAT32_ = 0x0C,
        FAT16__ = 0x0E,
        NTFS = 0x07,
        exPart = 0x0F,
        Linux_swap = 0x82,
        Linux = 0x83,
        Linux_LVM = 0x8E,
        RAW = 0xFF,
    };
    struct {
        unsigned start;
        unsigned part_size; //sectors
        unsigned sectors_per_block;
        System_ID system_ID;
    }part_info[4];

private:
    Cluster *disk;
    
public:
    bool has_MBR;
    DISK_PART(Cluster *disk_choose);

    void make_MBR();

    unsigned make_part(bool can_boot, unsigned from, unsigned to);

    void init_part(System_ID system_id, unsigned part_id);

    unsigned char get_system_id(unsigned part_id);

    void read(unsigned short *buf, unsigned part_id, unsigned start_block, unsigned blocks_read);

    void write(unsigned short *buf, unsigned part_id, unsigned start_block, unsigned blocks_write);

    void read_sec(unsigned short *buf, unsigned part_id, unsigned start_sec, unsigned counts);

    void set_block(unsigned part_id, unsigned sectors_per_block);
};

#endif