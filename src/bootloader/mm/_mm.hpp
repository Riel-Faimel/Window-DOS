#include "mm.hpp"
#include <start/BIOSinfo.hpp>
#include <drv/screen/screen_srv.hpp>

#define MemClusterSize 4096
#define SuperBlockMagic 'RSBM'
#define PageHeadMagic 'RPHM'
#define ObjectHeadGuard 'HG'
#define ObjectTailGuard 'TG'

#pragma pack(push, 1)
struct mmap_struc {
    u32 base_low;
    u32 base_high;
    u32 len_low;
    u32 len_high;
    enum Type : u32 {
        usable = 1,
        reserved = 2,
        ACPI = 3,
        NVS = 4,
        bad = 5,
    } type;
};
#pragma pack(pop)
extern "C" {
    inline mmap_struc *mmap_struc_base = (mmap_struc *)0x5000;
    extern PhysicalPage::mem_list* __boot_loader_end;
}

constexpr u16 size_classes[SIZE_CLASS_COUNT] = {
    16, 24, 32, 48, 64, 96, 128, 192, 256, 
    384, 512, 768, 1024, 1536, 2048, 3072, 4096
};