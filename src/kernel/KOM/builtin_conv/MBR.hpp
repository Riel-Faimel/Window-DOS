#ifndef __kernel_KOM_builtin_conv_MBR_hpp
#define __kernel_KOM_builtin_conv_MBR_hpp

#include <interface/knlobj/converter.hpp>

namespace builtin_converter{
        
class MBR : public Converter {
#pragma pack(push, 1)
    struct PE {
        u8 boot_flag;
        u8 start_head;
        u8 start_sector;
        u8 start_cylinder;
        u8 system_id;
        u8 end_head;
        u8 end_sector;
        u8 end_cylinder;
        u16 start_LBA_low;
        u16 start_LBA_high;
        u16 sector_count_low;
        u16 sector_count_high;
    };
    union mbr{
        u16 buf[256];
        struct {
            u8 boot[446];
            PE part[4];
            u16 sign = 0xAA55;
        };
    };
#pragma pack(pop)
public:
    MatchDegree touch(KernelObj *kobj) override ;
    void conv(KernelObj *kobj) override ;
};

} // namespace builtin_converter

#endif
