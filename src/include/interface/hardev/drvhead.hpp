#ifndef __include_interface_drvhead_hpp
#define __include_interface_drvhead_hpp

#include <global/type.hpp>
#include <interface/hardev/device.hpp>

#pragma pack(push, 1)
struct ModHead {
    const char magic[4];
    enum class ModType : u16 {
        PDevDrv,
        KrnlObj,
        SrvDev,
    } mod_type;

    constexpr ModHead(ModType type): magic{'K', 'M', 'H', 'M'}, mod_type{type} {}
};
struct PDevDrv : public ModHead {
    DeviceInfo::ClassType cls_t;
    ProgIFIndex pid;
    DeviceInfo::DeviceType dev_t;
    DriverConstr drvc;
    enum class DrvType : u8 {
        ProgIF,
        Dev,
    } drv_t;
};
struct KrnlObj : public ModHead {};
struct SrvDev : public ModHead {};
#pragma pack(pop)


#endif