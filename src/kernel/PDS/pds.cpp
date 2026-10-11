#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "_pds.hpp"

//#include <TL/condi_tranv>

PeripheralDeviceSpace *pdevspace;

PeripheralDeviceSpace::PeripheralDeviceSpace(): database{}, busspace{} {
    pdevspace = this;
    append(new ISA_space{});
    append(new PCI_space{});
    append(new USB_space{});
}

int PeripheralDeviceSpace::append(Enumerator *enumer) {
    static u16 bustypecount = 0;
    enumer->probe();
    busspace.append(EnumerNode{
        BusIndex{bustypecount}, enumer
    });
    return ++bustypecount;
}
int PeripheralDeviceSpace::append(BusIndex bid, ClassType cid, ProgIFIndex pid, const DeviceConfig &dcfg) {
    DebugInfo(cout << "append one device\r\n";)
    for (auto prog : database.filter([=](const DDR::PifNode &pnd)->bool{
        return (pnd.bus == bid) || (pnd.cls == cid) || (pnd.id == pid);
    })) {
        DebugInfo(cout << "Got one prog\r\n";)
    }
    auto dev_p = new DDR::PifNode {bid, cid, pid, {}, nullptr};
}
int PeripheralDeviceSpace::append(BusIndex bid, ClassType cid, ProgIFIndex pid, DriverConstr dcon) {
    ;
}
int PeripheralDeviceSpace::append(BusIndex bid, ClassType cid, ProgIFIndex pid, DeviceIndex did, DriverConstr dcon) {
    for (auto dev : database.filter(&always_true).filter(&always_true)) {
        ;
    }
}