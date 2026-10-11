#ifndef __kernel_drv_fs_DFRT_dfrt_struc_hpp
#define __kernel_drv_fs_DFRT_dfrt_struc_hpp

#include <global/type.hpp>

union LDBR { // Logical Disk Boot Recored
    struct {
        u8 jmp[3];
        u8 oem[8];

        u16 bytes_per_sector;
        u8 sectors_per_cluster;
        u16 reserved_sectors; // from vLBA 0 to FAT1 start
        u8 fat_table_count;
        u16 root_entry_num;
        u16 total_sectors_16;
        u8 medis_type_descriptor;
        u16 sector_per_fat_table;
        u16 sectors_per_track;
        u16 num_heads;
        u32 hidden_sectors; // from phyLBA 0 to vLBA 0
        u32 total_sectors_32;

        u32 mfrt_cluster_low;
        u32 mfrt_cluster_high;
        u32 mfrt_mirror_cluster_low;
        u32 mfrt_mirror_cluster_high;
        u32 cluster_per_recored;
        u32 pass_code;
        u32 _re1[2];

        u8 boot_code[440];
    };
    u16 buf[256];
};

/**
 * File Recored Cluster:
 * | ====
 * | prev ptr of Cluster address
 * | {48 bits for address, 16bits for total cluster count recored in this cluster low}
 * | ====
 * | next ptr of cluster address
 * | {48 bits for address, 16bits for total cluster... high}
 * | ====
 * | FileRecored items
 * | ...
 * | ====
 */

struct FileRecored {
    // cluster count can't more then LBA count
    u32 cluster_low;
    u16 cluster_high; 
    u16 cluster_count;
};

constexpr FileRecored end_recored = {0, 0, 0};
constexpr FileRecored full_seg_recored = {1, 0/*, full segment count*/}; 

/**
 * $MFRT:
 * | ====
 * | MFRT Head
 * | ====
 * | $Free file recored item
 * | ====
 * | FRT items
 * | ...
 * | ====
 */

struct MFRThead {
    u32 pass_code; // check with LDBR
};

struct FRTitem {
    u32 file_recored_cluster_start_low;
    u32 file_recored_cluster_start_high;
};


#endif