#ifndef __include_interface_knlobj_logicaldisk_hpp
#define __include_interface_knlobj_logicaldisk_hpp

#include <global/type.hpp>
#include <string>

struct DiskInfo {
    // storage unit information
    size_t su_start; // index start
    u64 su_num; // unit counts
    size_t su_size; // bytes per unit
    size_t type = 0;
};

struct PhysicalDiskInfo : DiskInfo {
    u8 address_mode; // | CHS(bit1) | LBA28(bit2) | LBA48(bit3)
    bool PIO_supported;
    char model[41];
    u8 device; 
    u16 CHS_cylinders;
    u16 CHS_heads;
    u16 CHS_sectors_per_track;
    u32 multi_count;
};

struct FileInfo : DiskInfo {
    String name;
    u16 create_date;
    u16 create_time;
    u16 write_date;
    u16 write_time;
    u16 access_date;

    u32 xattr1;
    u32 xattr2;
};

#endif