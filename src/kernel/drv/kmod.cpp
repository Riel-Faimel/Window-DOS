#include "_kmod.hpp"

KernelModule *kmod = nullptr;

template<>
size_t hash(const size_t &i) { return i; }

KernelModule::KernelModule():
fs{} {
    fs[14] = create_fat_fs;
    kmod = this;
}

