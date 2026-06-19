#ifndef __bootloader_LAS_SSS_hpp
#define __bootloader_LAS_SSS_hpp

#include <LAS/LAS.hpp>
#include <LAS/PM.hpp>

struct StorageSubSystem {
    LAS las;
    PM pm;

    StorageSubSystem():
    las{}, pm{} {}
};


#endif