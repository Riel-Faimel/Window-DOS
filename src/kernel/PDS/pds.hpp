#ifndef __kernel_PDS_pds_hpp
#define __kernel_PDS_pds_hpp

/**
 * PeripheralDeviceSpace (Physical Driver Space) is a space for all physical drivers
 * it is manage enumerator objects and provide driver queries and API for caller 
 * 
 * Abort database, PDS is more append/remove, fewer check/view
 */
#include <global/type.hpp>
#include <HL/container>
#include <HL/map>
#include <HL/condi_tranv>
#include <string>
#include <interface/hardev/enumer.hpp>
#include <interface/hardev/device.hpp>

class PeripheralDeviceSpace {
    class DeviceNode {
        struct Instance {
            DeviceInfo *info;
            DeviceDriver *drvInstance;

            Instance(DeviceInfo *info_): info{info_}, drvInstance{nullptr} {}
        };
    public:
        DeviceInfo::DeviceType dev_t;
        rtl::list<Instance> in_l;
        DriverConstr drv;
    };
    struct ProgramInFace {
        Enumerator::EnumerIndex bus_id;
        DeviceInfo::ClassType cls_t;
        ProgIFIndex prog_t;
        rtl::list<DeviceNode> dev_l;
        DriverConstr drv;
    };
    struct EnumerNode {
        Enumerator::EnumerIndex id;
        Enumerator *enumr;
    };
    class RecordDB {
        rtl::map<ProgIFIndex, ProgramInFace> prog_r;
    };

    rtl::array<EnumerNode> busspace;
    rtl::list<ProgramInFace> storage;
    Enumerator::EnumerIndex bc; // bus index counter
public:
    PeripheralDeviceSpace();

    // append bus
    Enumerator::EnumerIndex append(Enumerator *);
    // append device
    int append(DeviceInfo *);
    // append device
    int append(ProgIFIndex, DeviceInfo::DeviceType, DeviceInfo *);
    // append drvier for prog if
    int append(ProgIFIndex, DriverConstr);
    // append driver for device
    int append(ProgIFIndex, DeviceInfo::DeviceType, DriverConstr);
};

extern PeripheralDeviceSpace *pdevspace;

#endif