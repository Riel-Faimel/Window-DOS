#ifndef __bootloader_LAS_SSS_hpp
#define __bootloader_LAS_SSS_hpp

#include <LAS/LAS.hpp>
#include <LAS/PM.hpp>

struct StorageSubSystem {
    LAS las;
    PM pm;

    inline StorageSubSystem():
    las{}, pm{} {}
};


#endif