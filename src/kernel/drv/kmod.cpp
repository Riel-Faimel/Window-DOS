#include "_kmod.hpp"

KernelModule *kmod = nullptr;

template<>
size_t hash(const size_t &i) { return i; }

KernelModule::KernelModule():
fs{} {
    fs[14] = create_fat_fs;
    kmod = this;
}

void KernelModule::reg_insmod() {
    ;
}

void KernelModule::probe() {
    counter = pdevspace->apply_pifid(16);
    (*pdevspace)[""] = (new ISA_space{});
    (*pdevspace)[""] = (new PCI_space{});
}
