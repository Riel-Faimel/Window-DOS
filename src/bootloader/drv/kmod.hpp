#ifndef __bootloader_kmod_kmod_hpp
#define __bootloader_kmod_kmod_hpp

#include <TL/map>
#include <interface/dev_cls/cluster.hpp>
#include <interface/dev_cls/device.hpp>

bool operator == (const DeviceConfig &, const DeviceConfig &);

struct KernelModule{
    template<hashable k, typename v>
    using map = rtl::map<k, v>;

    map<DeviceConfig, void (*)(DeviceConfig &)> driver;
    map<size_t, Cluster *(*)(LogicalDisk *)> fs;
    KernelModule();
};

extern KernelModule *kmod;

#endif