#ifndef __bootloader_drv_disk_hpp
#define __bootloader_drv_disk_hpp
#include <IDT_L/idt.hpp>

class DISK_ {
public:
    virtual void read(unsigned short *, unsigned, unsigned char) = 0;
    virtual void write(unsigned short *, unsigned int, unsigned char) = 0;

    virtual void *info() = 0;
};

#endif