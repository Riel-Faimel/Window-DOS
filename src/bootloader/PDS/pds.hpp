#ifndef __bootloader_PDS_pds_hpp
#define __bootloader_PDS_pds_hpp

/**
 * PeripheralDeviceSpace (Physical Driver Space) is a space for all physical drivers
 * it is manage enumerator objects and provide driver queries and API for caller 
 */

#include "pdsdef.hpp"
#include <TL/container>
#include <interface/enumer.hpp>
#include <interface/dev_cls/device.hpp>

using DriverConstr = void *(*)(DeviceConfig &);

struct DeviceNode {
    DeviceIndex did;
    void *config;
    void *drvInstance;
    DriverConstr *drv;
};

struct ProgramInFace {
    BusIndex bus;
    ClassType cls;
    ProgIFIndex id;
    rtl::list<DeviceNode> dev;
    DriverConstr *drv;
};

struct EnumerNode {
    BusIndex id;
    Enumerator *enumr;
};

template<u32 N>
class condi_tranv;

using DeviceSpace_t = rtl::list<ProgramInFace>;
struct PeripheralDeviceSpace {
    PeripheralDeviceSpace();
    void probe();

    // append bus
    int append(Enumerator *);
    // append device
    int append(BusIndex, ClassType, ProgIFIndex, const DeviceConfig &);
    // append drvier for prog if
    int append(BusIndex, ClassType, ProgIFIndex, DriverConstr);
    // append driver for device
    int append(BusIndex, ClassType, ProgIFIndex, DeviceIndex, DriverConstr);
private:
    DeviceSpace_t space;
    rtl::array<EnumerNode> busspace;

    condi_tranv<0u> tranv();
};

#endif