#ifndef __bootloader_drv_disk_SATA_sata_hpp
#define __bootloader_drv_disk_SATA_sata_hpp
#include <drv/disk/disk.hpp>

class SATA : public Cluster {
    void check();
    void read_LBA(unsigned short *, unsigned, unsigned char);
    void write_LBA(unsigned short *, unsigned int, unsigned char);
};

#endif