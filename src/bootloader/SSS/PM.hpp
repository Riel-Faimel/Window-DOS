#ifndef __bootloader_DLS_PM_hpp
#define __bootloader_DLS_PM_hpp

/**
 * PM (Partition Manager) is a space for logical drivers
 * it is create more logical drivers from physical drivers and try to give file system to them
 */

#include <SSS/PM_.hpp>
#include <interface/dev_cls/logicaldisk.hpp>
#include <TL/container>

class PM {
#pragma pack(push, 1)
    struct PE {
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
    };
    union MBR{
        unsigned short buf[256];
        struct {
            unsigned char boot[446];
            PE part[4];
            unsigned short sign;
        };
    };
#pragma pack(pop)

    enum class System_ID : unsigned char {
        NULL = 0x00,
        FAT12 = 0x01,
        FAT16_ = 0x04,
        FAT16 = 0x06,
        FAT32 = 0x0B,
        FAT32_ = 0x0C,
        FAT16__ = 0x0E,
        NTFS = 0x07,
        exPart = 0x0F,
        Linux_swap = 0x82,
        Linux = 0x83,
        Linux_LVM = 0x8E,
        RAW = 0xFF,
        Re = 0xDA,
    };

    rtl::list<LogicalDisk *> disk_stack;
public:
    PM();
    ~PM() = default;

    void include(LogicalDisk *);
    void resolve(LogicalDisk *);
};

#endif