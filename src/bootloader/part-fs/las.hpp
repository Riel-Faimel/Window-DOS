#ifndef __bootloader_part_fs_las_hpp
#define __bootloader_part_fs_las_hpp
#include <lib/cppstdlib/string>
#include <part-fs/part-fs.hpp>
#include <drv/disk/disk.hpp>
#define __independent_lib_Using_template_container
#include <TL/idlib>
class CenterShell;

class LAS{
    struct DiskInfo{
        DISK_ *driver;
        DISK_PART *partitions;
        /**
         * Disk driver and it's part manager
         */
        unsigned disk_id;
    };
    struct DriveInfo{
        FAT16 *fs;
        String driver_letter;
        /**
         * File system and it's driver letter
         */
        unsigned disk_part_uid;
        /**
         * uid is
         * disk id | part id
         * <- 2Bs->|<-2Bs ->
         */
    };
    rtl::array<DiskInfo> disks;
    rtl::array<DriveInfo> drivers;

    void *dealing;// disk or filesystem which dealing
    unsigned char dealing_id;
    friend inline void mkfs_(String);
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

    void mkfs_FAT16(unsigned disk_part_uid);

    bool choose_disk(String &);

    void reg_cmd(CenterShell *);
    void set_letter(unsigned disk_part_uid, String letter);
    unsigned set_part(unsigned disk_id, unsigned from_LBA, unsigned to_LBA, String le = {});
    void show_driver();
};

extern LAS *linear_address_space;
#endif