#ifndef __include_interface_device_hpp
#define __include_interface_device_hpp

#include <global/type.hpp>
#include <interface/enumer.hpp>

// global defination
enum class InsProgifID : u32 {
    IDE,
    AHCI,
    end,
};

class BusIndex {
    u16 id = 0;
public:
    BusIndex() = default;
    constexpr explicit BusIndex(u16 id_): id{id_} {}
    constexpr explicit operator u16 () const { return id; }

    bool operator == (const BusIndex &oth) const { return id == oth.id; }
    BusIndex &operator ++ () { id++; return *this; }
    BusIndex operator ++ (int) { return BusIndex{id++}; }
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
class ProgIFType {
    u32 id;
public:
    constexpr explicit ProgIFType(u32 id_): id{id_} {}
    constexpr explicit operator u32 () const { return id; }

    bool operator == (const ProgIFType &oth) const { return id == oth.id; }
    ProgIFType &operator += (u32 add) { id += add; return *this; }
};
class DeviceType {
    u32 id;
public:
    constexpr explicit DeviceType(u32 id_): id{id_} {}

    bool operator == (const DeviceType &oth) const { return id == oth.id; }
};
struct DeviceConfig {void *cfg;};

using DriverConstr = void *(*)(void *);

/**
 * Enumerator submit the struct to PDS
 * Enumerator know:
 *      self id allocated by PDS
 *      device class
 *      device program interface
 *      device id
 *          # up for device classifying
 * 
 *      Enumerator private config
 *      self
 *          # Enumerator local information
 */
struct DeviceInfo {
    // id
    BusIndex bus; // from enumer run-time
    ClassType cls; // from enum class
    ProgIFType progif; // from 
    DeviceType device; // from vendoor, device, reversion hash

    // bus config
    void *config;
    Enumerator *enumer = nullptr;
};

#endif