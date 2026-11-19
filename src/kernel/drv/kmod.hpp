#ifndef __kernel_kmod_kmod_hpp
#define __kernel_kmod_kmod_hpp

#include <HL/map>
#include <interface/srvdev/cluster.hpp>
#include <interface/hardev/device.hpp>
#include <interface/knlobj/knlobj.hpp>

struct KernelModule : Enumerator {
    rtl::map<size_t, Cluster *(*)(LogicalDisk *)> fs;
    KernelModule();

    void probe() override;
    // 模块表不是硬件总线，没有可配置的门控。
    void set_mod(void *) override {}
private:
    void reg_insmod();

    ProgIFIndex counter{0};
};

extern KernelModule *kmod;

#endif