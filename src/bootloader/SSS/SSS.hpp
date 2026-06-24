#ifndef __bootloader_DLS_SSS_hpp
#define __bootloader_DLS_SSS_hpp

#include <SSS/DLS.hpp>
#include <SSS/PM.hpp>

struct StorageSubSystem {
    DLS las;
    PM pm;

    inline StorageSubSystem():
    las{}, pm{} {}
};


#endif