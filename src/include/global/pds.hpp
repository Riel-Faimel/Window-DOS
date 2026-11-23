#pragma once
#include <interface/hardev/pift.hpp>
#include <interface/hardev/enumer.hpp>

namespace pds {
    
enum class Result : int {
    NoError,
    
    BadArgs,
    NoSpace,
};

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
    ProgIFIndex progif;

    // id
    Enumerator::EnumerIndex bus; // from enumer run-time
    enum class ClassType : u16 {
        storage,
        network,
        graph,
        caculate,
        input,
        output,
        nul = (u16)-1,
    } cls; // from enum class
    class DeviceType {
        u32 id;
    public:
        constexpr explicit DeviceType(u32 id_): id{id_} {}

        bool operator == (const DeviceType &oth) const { return id == oth.id; }
        operator u32() const { return id; }
    } device; // from vendoor, device, reversion hash

    // bus config
    void *config;
    Enumerator *enumer = nullptr;
};

} // namespace pds 