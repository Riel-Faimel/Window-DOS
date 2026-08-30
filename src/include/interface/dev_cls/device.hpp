#ifndef __include_interface_device_hpp
#define __include_interface_device_hpp

#include <global/type.hpp>
#include <interface/enumer.hpp>

// global defination
#define _GD_storage_IDE_disk 0
#define _GD_storage_SATA_disk 1
#define _GD_storage_NVMe_disk 2
#define _GD_network 0

struct DeviceConfig {
    enum class BUS_TYPE : unsigned {
        PCI,
        USB,
        nul = (unsigned)-1,
    } bus_type = BUS_TYPE::nul;
    enum class CLASS_TYPE : unsigned {
        storage,
        network,
        graph,
        caculate,
        input,
        output,
        nul = (unsigned)-1,
    } class_type = CLASS_TYPE::nul;
    size_t device_type = -1;
    void *config = nullptr;
    Enumerator *enumer = nullptr;
};

#endif