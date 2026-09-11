#include "_pds.hpp"

PeripheralDeviceSpace::PeripheralDeviceSpace(): space{} {
    space.append(new ISA_space{});
    space.append(new PCI_space{});
    space.append(new USB_space{});
}

void PeripheralDeviceSpace::remove(unsigned long long tpid){}

void PeripheralDeviceSpace::probe() {
    for (auto &enumer : space) {
        enumer->probe();
    }
}

void PeripheralDeviceSpace::set_driver() {
    for (auto &enumer : space) {
        enumer->set_driver();
    }
}
