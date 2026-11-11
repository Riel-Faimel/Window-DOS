#ifndef __kernel_kmod_kmod_hpp
#define __kernel_kmod_kmod_hpp

#include <HL/map>
#include <interface/dev_cls/cluster.hpp>
#include <interface/dev_cls/device.hpp>

struct KernelModule : Enumerator {
    rtl::map<size_t, Cluster *(*)(LogicalDisk *)> fs;
    KernelModule();

    void probe() override;
private:
    void reg_insmod();

    ProgIFType counter{0};
};

extern KernelModule *kmod;

#endif