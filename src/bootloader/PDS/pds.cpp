#include "_pds.hpp"

PDS::PDS(): pci{false}, usb() {}

void PDS::remove(unsigned long long tpid){}

void PDS::probe() {
    pci.probe();
    usb.probe();
}

void PDS::set_driver() {
    usb.set_driver();
    pci.set_driver();
}
