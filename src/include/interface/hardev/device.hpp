#ifndef __include_interface_device_hpp
#define __include_interface_device_hpp

#include <global/type.hpp>
#include <global/pds.hpp>
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
    virtual ~DeviceDriver() = default;
};

using DriverConstr = DeviceDriver *(*)(pds::DeviceInfo *);

#endif