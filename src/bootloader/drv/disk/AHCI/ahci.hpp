#ifndef __bootloader_drv_disk_AHCI_ahci_hpp
#define __bootloader_drv_disk_AHCI_ahci_hpp

#include <interface/dev_cls/cluster.hpp>

class SATA : public Cluster {
    void check();
    void read_LBA(unsigned short *, unsigned, unsigned char);
    void write_LBA(unsigned short *, unsigned int, unsigned char);
};

#endif