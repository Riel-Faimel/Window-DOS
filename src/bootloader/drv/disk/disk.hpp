#ifndef __bootloader_drv_disk_hpp
#define __bootloader_drv_disk_hpp
#define _BITS_32
#include <global/type.hpp>
#include <interface/cluster.hpp>

struct DISK_INFO : Cluster::Cluster_Info{
    int total_sectors;
    int LBA_support;
    int PIO_supported;
    char model[41];
    u8 device; 
    u16 CHS_cylinders;
    u16 CHS_heads;
    u16 CHS_sectors_per_track;
    u32 multi_count;
};

#endif