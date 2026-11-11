#ifndef __kernel_drv_mod_head_hpp
#define __kernel_drv_mod_head_hpp

#include <global/type.hpp>
#include <interface/dev_cls/device.hpp>

#pragma pack(push, 1)
struct ModHead {
    const char magic[4];
    enum class ModType : u16 {
        PDevDrv,
        Kobj,
        CDev,
        SDev,
    } mod_type;

    constexpr ModHead(ModType type): magic{'K', 'M', 'H', 'M'}, mod_type{type} {}
};
struct PDevDrv {
    ProgIFType pid;
    DeviceType dev_t;
    DriverConstr drvc;
    ClassType cls_t;
    enum class DrvType : u8 {
        ProgIF,
        Dev,
    } drv_t;
};
#pragma pack(pop)


#endif