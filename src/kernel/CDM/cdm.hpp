#ifndef __kernel_CDM_cdm_hpp
#define __kernel_CDM_cdm_hpp

#include <CDM/DLS.hpp>
#include <CDM/PM.hpp>

struct ClusterDeviceManager {
    DLS las;
    PM pm;

    ClusterDeviceManager():
    las{}, pm{} {}
};


#endif