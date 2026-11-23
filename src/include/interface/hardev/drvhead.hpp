#ifndef __include_interface_drvhead_hpp
#define __include_interface_drvhead_hpp

#include <global/type.hpp>
#include <interface/hardev/device.hpp>
#include <interface/srvdev/cluster.hpp>
#include <interface/wm/syscall.hpp>
#include <interface/knlobj/knlobj.hpp>
#include <interface/knlobj/converter.hpp>

#define InsideModuleRecord constinit static __attribute__((used, section(".insidemods"), aligned(1)))

#pragma pack(push, 1)
struct ModHead {
    const char magic[4];
    enum class ModType : u16 {
        PDevDrv,
        KrnlObj,
        SrvDev,
        EnumerMod,
        SyscallMod,
    } mod_type;

    constexpr ModHead(ModType type): magic{'K', 'M', 'H', 'M'}, mod_type{type} {}
};
struct PDevDrv : public ModHead {
    ProgIFIndex progid;
    pds::DeviceInfo::ClassType cls_type;
    pds::DeviceInfo::DeviceType device_type;
    DriverConstr drv_construc;
    enum class DrvType : u8 {
        ProgIF,
        Dev,
    } drv_type;

    constexpr PDevDrv(
        ProgIFIndex progid_, pds::DeviceInfo::ClassType cls_type_, 
        pds::DeviceInfo::DeviceType device_type_, DriverConstr drv_construc_
    ): ModHead{ModType::PDevDrv}, progid{progid_}, cls_type{cls_type_}, device_type{device_type_}, 
    drv_construc{drv_construc_}, drv_type{DrvType::Dev} {}
    constexpr PDevDrv(
        ProgIFIndex progid_, pds::DeviceInfo::ClassType cls_type_, DriverConstr drv_construc_
    ): ModHead{ModType::PDevDrv}, progid{progid_}, cls_type{cls_type_}, device_type{0}, 
    drv_construc{drv_construc_}, drv_type{DrvType::ProgIF} {}
};
struct KrnlObj : public ModHead {
    Converter *(*conv_construc)();

    constexpr KrnlObj(Converter *(*conv_construc_)()):
    ModHead{ModType::KrnlObj}, conv_construc{conv_construc_} {}
};
struct EnumerMod : public ModHead {
    Enumerator *(*mainbody)();

    constexpr EnumerMod(Enumerator *(*mainbody_)()): 
    ModHead{ModType::EnumerMod}, mainbody{mainbody_} {}
};
struct SyscallMod : public ModHead {
    u32 interrupt_index;
    SyscallHandler *(*create)();

    constexpr SyscallMod(u32 intid, SyscallHandler *(*create_)()):
    ModHead{ModType::SyscallMod}, interrupt_index{intid}, create{create_} {}
};
#pragma pack(pop)


#endif