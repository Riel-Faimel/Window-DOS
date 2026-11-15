#ifndef __include_interface_device_hpp
#define __include_interface_device_hpp

#include <global/type.hpp>
#include "enumer.hpp"
#include "pift.hpp"

class DeviceDriver {
public:
    /**
     * work():
     * 驱动把自己的功能以内核对象的形式发布进kom
     * off():
     * 驱动注销自己发布在kom里的功能
     */
    virtual void work() = 0;
    virtual void off() = 0;
};
using DriverConstr = DeviceDriver *(*)(void *);

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

#endif