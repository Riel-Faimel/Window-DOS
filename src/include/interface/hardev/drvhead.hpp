#ifndef __include_interface_drvhead_hpp
#define __include_interface_drvhead_hpp

#include <global/type.hpp>
#include <interface/hardev/device.hpp>
#include <interface/srvdev/cluster.hpp>
#include <interface/syscall.hpp>

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
    DeviceInfo::ClassType cls_type;
    DeviceInfo::DeviceType device_type;
    DriverConstr drv_construc;
    enum class DrvType : u8 {
        ProgIF,
        Dev,
    } drv_type;

    constexpr PDevDrv(
        ProgIFIndex progid_, DeviceInfo::ClassType cls_type_, 
        DeviceInfo::DeviceType device_type_, DriverConstr drv_construc_
    ): ModHead{ModType::PDevDrv}, progid{progid_}, cls_type{cls_type_}, device_type{device_type_}, 
    drv_construc{drv_construc_}, drv_type{DrvType::Dev} {}
    constexpr PDevDrv(
        ProgIFIndex progid_, DeviceInfo::ClassType cls_type_, DriverConstr drv_construc_
    ): ModHead{ModType::PDevDrv}, progid{progid_}, cls_type{cls_type_}, device_type{0}, 
    drv_construc{drv_construc_}, drv_type{DrvType::ProgIF} {}
};
struct KrnlObj : public ModHead {};
struct SrvDev : public ModHead {
    u32 fs_id;
    Cluster *(*fs_construc)(LogicalDisk *);

    constexpr SrvDev(u32 fs_id_, Cluster *(*fs_construc_)(LogicalDisk *)): 
    ModHead{ModType::SrvDev}, fs_id{fs_id_}, fs_construc{fs_construc_} {}
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