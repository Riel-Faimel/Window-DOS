#ifndef __kernel_PDS_pds_hpp
#define __kernel_PDS_pds_hpp

/**
 * PeripheralDeviceSpace (Physical Driver Space) is a space for all physical drivers
 * it is manage enumerator objects and provide driver queries and API for caller 
 * 
 * Abort database, PDS is more append/remove, fewer check/view
 * 
 *  struct:
 * 
 *   ProgramInFace
 *   ├── drv
 *   └── dev_l   DeviceNode
 *               ├── drv
 *               └── in_l   Instance
 *                           ├── config
 *                           └── drvInstance
 */
#include <global/type.hpp>
#include <global/pds.hpp>
#include <HL/container>
#include <HL/map>
#include <HL/condi_tranv>
#include <string>
#include <interface/hardev/enumer.hpp>
#include <interface/hardev/device.hpp>

class PeripheralDeviceSpace {
    using DeviceInfo = pds::DeviceInfo;
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
    class RecordDB {
        rtl::map<ProgIFIndex, ProgramInFace> prog_r;
    };

    rtl::array<Enumerator *> busspace;
    rtl::list<ProgramInFace> storage;
    Enumerator::EnumerIndex bc; // bus index counter
public:
    PeripheralDeviceSpace();

    // append bus
    Enumerator::EnumerIndex append(Enumerator *);
    // append device
    pds::Result append(DeviceInfo *);
    // append device
    pds::Result append(ProgIFIndex, Enumerator::EnumerIndex, DeviceInfo::DeviceType, DeviceInfo *);
    // append drvier for prog if
    pds::Result append(ProgIFIndex, DriverConstr);
    // append driver for device
    pds::Result append(ProgIFIndex, DeviceInfo::DeviceType, DriverConstr);
    // get dev status
    // void view();
};

extern PeripheralDeviceSpace *pdevspace;

#endif