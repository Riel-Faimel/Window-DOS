#ifndef __bootloader_drv_disk_SATA_sata_hpp
#define __bootloader_drv_disk_SATA_sata_hpp
#include <drv/disk/disk.hpp>

class SATA : public DISK_{
    void check();
    void read(unsigned short *, unsigned, unsigned char);
    void write(unsigned short *, unsigned int, unsigned char);
};

#endif