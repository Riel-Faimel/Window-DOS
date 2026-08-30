#ifndef __bootloader_PDS_pds_hpp
#define __bootloader_PDS_pds_hpp

/**
 * PeripheralDeviceSpace (Physical Driver Space) is a space for all physical drivers
 * it is manage enumerator objects and provide driver queries and API for caller 
 */

#include <PDS/PCI/pci.hpp>
#include <PDS/USB/usb.hpp>

class PeripheralDeviceSpace {
private:
    PCI_space pci;
    USB_space usb;
public:
    PeripheralDeviceSpace();
    void probe();
    void set_driver();
    void remove(unsigned long long);
};

#endif