#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "_pds.hpp"

PeripheralDeviceSpace *pdevspace;

PeripheralDeviceSpace::PeripheralDeviceSpace(): 
busspace{}, storage{}, database{storage}, pc{(u32)InsProgifID::end}, bc{0} { pdevspace = this; }

Enumerator *&PeripheralDeviceSpace::operator [] (String bus_name) {
    /*return busspace[bus_name]*/;
}
int PeripheralDeviceSpace::append(DeviceInfo *dev) {
    append(dev->bus, dev->cls, dev->progif, dev->device, dev->config);
    delete dev;
    return 0;
}
int PeripheralDeviceSpace::append(BusIndex bid, ClassType cls_t, ProgIFType prog_t, DeviceType dev_t, void *dcfg) {
    for (auto prog : database
        .with([=](const ProgramInFace &pif){ return (pif.prog_t == prog_t); })
    ) {
        LocalDebugInfo(kout << (u16)bid << ", " << (u8)cls_t << ", " << (u32)prog_t << '\n';)
        for (auto devn : rtl::Filter<DeviceNode, rtl::list>{prog.dev_l}
            .with([=](const DeviceNode &dev) { return dev.dev_t == dev_t; })
        ) {
            devn.in_l.append({dcfg});
        }
        return 0;
    }
    LocalDebugInfo(kout << "append one device\r\n";)
    storage.append(ProgramInFace {bid, cls_t, prog_t, {}, nullptr});
    return 0;
}
int PeripheralDeviceSpace::append(BusIndex bid, ClassType cid, ProgIFType pid, DriverConstr dcon) {
    return 0;
}
int PeripheralDeviceSpace::append(BusIndex bid, ClassType cid, ProgIFType pid, DeviceType did, DriverConstr dcon) {
    return 0;
}
ProgIFType PeripheralDeviceSpace::apply_pifid(size_t size) {
    auto re = pc;
    pc += size;
    return re;
}
BusIndex PeripheralDeviceSpace::apply_busid() { return bc++; }