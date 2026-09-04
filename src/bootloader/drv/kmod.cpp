#include "_kmod.hpp"

KernelModule *kmod = nullptr;

template<>
size_t hash(const size_t &i) { return i; }
template<>
size_t hash(const DeviceConfig &cfg) {
    return (size_t)cfg.bus_type + (size_t)cfg.class_type * 137 + cfg.device_type * 137 * 137;
}
bool operator == (const DeviceConfig &_1, const DeviceConfig &_2) {
    return _1.bus_type == _2.bus_type && _1.class_type == _2.class_type && _1.device_type == _2.device_type;
}

KernelModule::KernelModule():
driver{}, fs{} {
    constexpr DeviceConfig pci_ide{
        DeviceConfig::BUS_TYPE::PCI,
        DeviceConfig::CLASS_TYPE::storage,
        _GD_storage_IDE_disk,
        0, 0
    };
    driver[pci_ide] = init_IDE_controller;
    fs[14] = create_fat_fs;
    kmod = this;
}

