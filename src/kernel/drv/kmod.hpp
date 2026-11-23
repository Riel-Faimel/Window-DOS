#ifndef __kernel_kmod_kmod_hpp
#define __kernel_kmod_kmod_hpp

#include <HL/map>
#include <interface/srvdev/cluster.hpp>
#include <interface/hardev/device.hpp>
#include <interface/knlobj/knlobj.hpp>
#include <interface/hardev/drvhead.hpp>

struct KernelModule : Enumerator {
    KernelModule();

    void probe() override;
    void set_mod(void *) override {}
private:
    void reg_insmod(ModHead&);

    ProgIFIndex counter{0};
};

extern KernelModule *kmod;

#endif