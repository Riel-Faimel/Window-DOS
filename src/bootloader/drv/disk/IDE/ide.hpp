#ifndef __bootloader_drv_disk_IDE_ide_hpp
#define __bootloader_drv_disk_IDE_ide_hpp
#include <IDT_L/idt.hpp> 
#include <drv/disk/disk.hpp>
#include "ide_.hpp"

class IDE_DISK : public LogicalDisk {
    friend class IDE_Channal;
    enum class Device{
        Master_Device = 0xE0,
        Slave_Device = 0xF0,
    };

    IDE_Channal *lock;
    DISK_INFO info_;
public:
    bool exist;
    IDE_DISK();
    ~IDE_DISK() = default;
    IDE_DISK(Device dev, IDT &idt, IDE_Channal *);

    IDE_DISK(IDE_DISK &) = delete;
    IDE_DISK& operator=(IDE_DISK&&);
    IDE_DISK& operator=(IDE_DISK&) = delete;
    IDE_DISK& operator=(const IDE_DISK&) = delete;

    void check();
    unsigned read(void *buf, unsigned LBA, unsigned, unsigned sectors_read);
    unsigned write(void *buf, unsigned LBA, unsigned, unsigned sectors_write);
    DISK_INFO* info(String);
    unsigned cmd(unsigned, String, void *argv, unsigned argc);
};
#endif