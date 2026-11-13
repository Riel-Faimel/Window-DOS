#ifndef __kernel_kmod_kmod_hpp
#define __kernel_kmod_kmod_hpp

#include <HL/map>
#include <interface/srvdev/cluster.hpp>
#include <interface/hardev/device.hpp>

struct KernelModule : Enumerator {
    rtl::map<size_t, Cluster *(*)(LogicalDisk *)> fs;
    KernelModule();

    void probe() override;
private:
    void reg_insmod();

    ProgIFIndex counter{0};
};

extern KernelModule *kmod;

#endif