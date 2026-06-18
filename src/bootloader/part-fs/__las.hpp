#ifndef __bootloader_part_fs_las_hpp
#define __bootloader_part_fs_las_hpp
#include <lib/cppstdlib/string>
#include <part-fs/fs.hpp>
#include <drv/disk/disk.hpp>
#define __independent_lib_Using_template_container
#include <TL/idlib>
class CenterShell;
class DISK_PART;

class LAS{
    struct DiskInfo{
        Cluster *driver;
        DISK_PART *partitions;
        /**
         * Disk driver and it's part manager
         */
        unsigned disk_id;
    };
    struct DriveInfo{
        FileSystem *fs;
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

    /*
    struct {
        void *dealing;// disk or filesystem which dealing
        unsigned char dealing_id;
        bool is_fs;
    };
    */
    String letter;
    friend inline void mkfs_(String);
    friend inline void __dir(String);
public:
    LAS();
    /**
     * goto PCI space to find disk and get driver object, 
     * then set partition object, if has FAT16 partition, 
     * set fat16 object, else RAW
     */

    bool choose_disk(String &);
    void show_driver();

    unsigned open(String);
    void close(unsigned );
    void read(unsigned , unsigned char *, unsigned, unsigned);
    void write(unsigned , unsigned char *, unsigned, unsigned);

    void set_letter(unsigned disk_part_uid, String letter);
    unsigned set_part(unsigned disk_id, unsigned from_LBA, unsigned to_LBA, String le = {});
    void reg_cmd(CenterShell *);

    /**
     * .sys:
     * 'LAFS' for head
     * follow is:
     * .sys.
     * construct function for 4 bytes.
     */
    void load_mod(void *address, u16 section);
};

extern LAS *linear_address_space;
#endif