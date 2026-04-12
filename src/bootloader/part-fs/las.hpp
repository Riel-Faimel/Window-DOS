#ifndef __bootloader_part_fs_las_hpp
#define __bootloader_part_fs_las_hpp
#include <lib/cppstdlib/string.hpp>
#include <part-fs/part-fs.hpp>
#include <drv/disk/disk.hpp>
class CenterShell;

class LAS{
    struct DiskInfo{
        DISK_ *driver;
        DISK_PART *partitions;
        FAT16 *fs;
        unsigned fs_num;
        String drvier_letter;
    };
    DiskInfo *disks;
    unsigned disk_num;

    void *dealing;// disk or filesystem which dealing
public:
    LAS();
    /**
     * goto PCI space to find disk and get driver object, 
     * then set partition object, if has FAT16 partition, 
     * set fat16 object, else RAW
     */

    unsigned open(String);
    void close(unsigned );

    void read(unsigned , unsigned char *, unsigned, unsigned);
    void write(unsigned , unsigned char *, unsigned, unsigned);

    void mkfs_FAT16(unsigned char disk_id, unsigned from_LBA, unsigned to_LBA);

    bool choose_disk(String &);

    void reg_cmd(CenterShell *);
};

extern LAS *linear_address_space;
#endif