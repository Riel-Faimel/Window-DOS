#ifndef __bootloader_start_BIOSinfo_hpp
#define __bootloader_start_BIOSinfo_hpp

#pragma pack(push, 1)
struct BootINFO {
    unsigned char boot_device;
    unsigned char mmap_size;
};
#pragma pack(pop)

inline BootINFO *boot_infomation = reinterpret_cast<BootINFO *>(0x7C00 + 320);

#endif