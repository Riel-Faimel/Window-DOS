#ifndef __bootloader_PDS_pdsdef_hpp
#define __bootloader_PDS_pdsdef_hpp

#include <global/type.hpp>

struct BusIndex {
    u16 id = 0;

    BusIndex() = default;
    explicit BusIndex(u16 id_): id{id_} {}
};
enum class ClassType : u16 {
    storage,
    network,
    graph,
    caculate,
    input,
    output,
    nul = (u16)-1,
};
struct ProgIFIndex {
    u32 id;
    
    explicit ProgIFIndex(u32 id_): id{id_} {}
};
struct DeviceIndex {
    u32 id;
    
    explicit DeviceIndex(u32 id_): id{id_} {}
};

#endif