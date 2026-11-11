#ifndef __kernel_PDS_pds_hpp
#define __kernel_PDS_pds_hpp

/**
 * PeripheralDeviceSpace (Physical Driver Space) is a space for all physical drivers
 * it is manage enumerator objects and provide driver queries and API for caller 
 * 
 * Abort database, PDS is more append/remove, fewer check/view
 */

#include "pdsdef.hpp"
#include <global/type.hpp>
#include <HL/container>
#include <HL/map>
#include <HL/condi_tranv>
#include <string>
#include <interface/enumer.hpp>
#include <interface/dev_cls/device.hpp>

struct XXX {
    XXX();

private:
    struct DeviceNode {
        // device id
        DeviceType device_typeID;
        // list<pair<device instance, driver instance>>
        rtl::list<rtl::pair<void *, void *>> InstanceList;
        // drv factry
        DriverConstr drv;
    };
    struct ProgIFNode {
        // progif id map to here
        // abort bus
        // abort class
        // list device information
        rtl::list<DeviceNode> DeviceList;
        // drv factry
        DriverConstr drv;
    };
    rtl::map<ProgIFType, ProgIFNode> device_record_space;
};

struct PeripheralDeviceSpace {
    PeripheralDeviceSpace();

    // append bus
    BusIndex append(Enumerator *);
    Enumerator *&operator [] (String );
    // append device
    int append(DeviceInfo *);
    // append device
    int append(BusIndex, ClassType, ProgIFType, DeviceType, void *);
    // append drvier for prog if
    int append(BusIndex, ClassType, ProgIFType, DriverConstr);
    // append driver for device
    int append(BusIndex, ClassType, ProgIFType, DeviceType, DriverConstr);
    // apply for id
    ProgIFType apply_pifid (size_t);
    BusIndex apply_busid ();
private:
    class DeviceNode {
        struct Instance {
            void *config;
            void *drvInstance;

            Instance(void *config_): config{config_}, drvInstance{nullptr} {}
            Instance(void *config_, void *drv_): config{config_}, drvInstance{drv_} {}
        };
    public:
        DeviceType dev_t;
        rtl::list<Instance> in_l;
        DriverConstr *drv;
    };
    struct ProgramInFace {
        BusIndex bus_id;
        ClassType cls_t;
        ProgIFType prog_t;
        rtl::list<DeviceNode> dev_l;
        DriverConstr *drv;
    };
    struct EnumerNode {
        BusIndex id;
        Enumerator *enumr;
    };
    class RecordDB {
        rtl::map<ProgIFType, ProgramInFace> prog_r;
    };

    rtl::array<EnumerNode> busspace;
    rtl::list<ProgramInFace> storage;
    rtl::Filter<ProgramInFace, rtl::list> database;
    ProgIFType pc; // Prog interface index counter
    BusIndex bc; // bus index counter
};

extern PeripheralDeviceSpace *pdevspace;

#endif