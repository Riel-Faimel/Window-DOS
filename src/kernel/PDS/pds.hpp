#ifndef __kernel_PDS_pds_hpp
#define __kernel_PDS_pds_hpp

/**
 * PeripheralDeviceSpace (Physical Driver Space) is a space for all physical drivers
 * it is manage enumerator objects and provide driver queries and API for caller 
 */

#include "pdsdef.hpp"
#include <TL/container>
#include <TL/concept>
#include <interface/enumer.hpp>
#include <interface/dev_cls/device.hpp>

using DriverConstr = void *(*)(DeviceConfig &);

class Device_DriverRecored;

template<typename T, typename P>
concept DevAcCon = requires (T t, const P &pif) {
    rtl::is_same_t<decltype(t(pif)), bool>;
};

class Device_DriverRecored {
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
    using DeviceSpace_t = rtl::list<ProgramInFace>;
    friend class PeripheralDeviceSpace;

    template<u32 N, DevAcCon<ProgramInFace> AC>
    class CondiView;
    template<typename C>
    class Iterator;
    class it;
public:
    using DevNode = DeviceNode;
    using PifNode = ProgramInFace;
    Device_DriverRecored() = default;

    void append(const ProgIFIndex &);

    template<DevAcCon<PifNode> AC>
    CondiView<0u, AC> filter(AC);
    it begin() const;
    it end() const;
private:
    DeviceSpace_t storage;
};

using DevNode = Device_DriverRecored::DevNode;
using PifNode = Device_DriverRecored::PifNode;

struct PeripheralDeviceSpace {
    PeripheralDeviceSpace();

    // append bus
    int append(Enumerator *);
    // append device
    int append(BusIndex, ClassType, ProgIFIndex, const DeviceConfig &);
    // append drvier for prog if
    int append(BusIndex, ClassType, ProgIFIndex, DriverConstr);
    // append driver for device
    int append(BusIndex, ClassType, ProgIFIndex, DeviceIndex, DriverConstr);
private:
    struct EnumerNode {
        BusIndex id;
        Enumerator *enumr;
    };

    Device_DriverRecored database;
    rtl::array<EnumerNode> busspace;
};

extern PeripheralDeviceSpace *pdevspace;

#endif