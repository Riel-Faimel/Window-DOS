#ifndef __bootloader_DLS_PM_hpp
#define __bootloader_DLS_PM_hpp

/**
 * PM (Partition Manager) is a space for logical drivers
 * it is create more logical drivers from physical drivers and try to give file system to them
 */

#include <global/type.hpp>
#include <SSS/PM_.hpp>
#include <interface/dev_cls/logicaldisk.hpp>
#include <TL/container>

class PM {
#pragma pack(push, 1)
    struct PE {
        u8 boot_flag;
        u8 start_head;
        u8 start_sector;
        u8 start_cylinder;
        u8 system_id;
        u8 end_head;
        u8 end_sector;
        u8 end_cylinder;
        u16 start_LBA_low;
        u16 start_LBA_high;
        u16 sector_count_low;
        u16 sector_count_high;
    };
    union MBR{
        u16 buf[256];
        struct {
            u8 boot[446];
            PE part[4];
            u16 sign = 0xAA55;
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