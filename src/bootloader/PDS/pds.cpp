#include "_pds.hpp"

PeripheralDeviceSpace::PeripheralDeviceSpace(): pci{false}, usb() {}

void PeripheralDeviceSpace::remove(unsigned long long tpid){}

void PeripheralDeviceSpace::probe() {
    pci.probe();
    usb.probe();
}

void PeripheralDeviceSpace::set_driver() {
    usb.set_driver();
    pci.set_driver();
}
