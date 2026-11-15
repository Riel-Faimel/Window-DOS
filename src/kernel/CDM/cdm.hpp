#ifndef __kernel_CDM_cdm_hpp
#define __kernel_CDM_cdm_hpp

#include <CDM/DLS.hpp>

struct ClusterDeviceManager {
    DLS las;

    ClusterDeviceManager():
    las{} {}
};


#endif