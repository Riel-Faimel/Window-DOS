#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "_pds.hpp"

PeripheralDeviceSpace::PeripheralDeviceSpace(): space{}, busspace{} {
    append(new ISA_space{});
    append(new PCI_space{});
    append(new USB_space{});
}

void PeripheralDeviceSpace::probe() {
    for (auto [_, enumer] : busspace) {
        enumer->probe();
    }
}

condi_tranv<0U> PeripheralDeviceSpace::tranv() { return {space}; }

int PeripheralDeviceSpace::append(Enumerator *enumer) {
    static u16 bustypecount = 0;
    busspace.append(EnumerNode{
        BusIndex{bustypecount}, enumer
    });
    return ++bustypecount;
}
int PeripheralDeviceSpace::append(BusIndex bid, ClassType cid, ProgIFIndex pid, const DeviceConfig &dcfg) {
    ;
}
int PeripheralDeviceSpace::append(BusIndex bid, ClassType cid, ProgIFIndex pid, DriverConstr dcon) {
    ;
}
int PeripheralDeviceSpace::append(BusIndex bid, ClassType cid, ProgIFIndex pid, DeviceIndex did, DriverConstr dcon) {
    for (auto dev : tranv().filter(always_true));
}
