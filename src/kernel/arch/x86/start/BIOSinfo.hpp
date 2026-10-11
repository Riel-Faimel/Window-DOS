#ifndef __kernel_start_BIOSinfo_hpp
#define __kernel_start_BIOSinfo_hpp

namespace arch {

#pragma pack(push, 1)
struct BootINFO {
    unsigned char boot_device;
    unsigned char mmap_size;
};
#pragma pack(pop)

inline BootINFO *boot_infomation = reinterpret_cast<BootINFO *>(0x7C00 + 320);
    
} // namespace arch 

#endif