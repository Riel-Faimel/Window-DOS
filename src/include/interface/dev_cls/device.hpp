#ifndef __include_interface_device_hpp
#define __include_interface_device_hpp

#include <global/type.hpp>
#include <interface/enumer.hpp>

// global defination
#define _GM_DeviceType_PATA 0
#define _GM_DeviceType_SATA 1
#define _GM_ProgramInterface_IDE 0
#define _GM_ProgramInterface_AHCI 1

struct DeviceConfig {
    void *config = nullptr;
    void *drv = nullptr;
};

struct DeviceInfo {
    using ProgramInterface = u32;

    enum class BUS_TYPE : u32 {
        ISA,
        PCI,
        USB,
        nul = (u32)-1,
    } bus_type = BUS_TYPE::nul;
    enum class CLASS_TYPE : u32 {
        storage,
        network,
        graph,
        caculate,
        input,
        output,
        nul = (u32)-1,
    } class_type = CLASS_TYPE::nul;
    struct DeviceType {
        u32 type = {(u32)-1};
    } device_type;
    ProgramInterface programinterface = (ProgramInterface)-1;
    void *config = nullptr;
    Enumerator *enumer = nullptr;
};

#endif