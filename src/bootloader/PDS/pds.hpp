#ifndef __bootloader_PDS_pds_hpp
#define __bootloader_PDS_pds_hpp

/**
 * PDS (Physical Driver Space) is a space for all physical drivers
 * it is manage enumerator objects and provide driver queries and API for caller 
 */

#include <interface/dev_cls/logicaldisk.hpp>
#include <TL/container>

class PCI_space;
class USB_space;

class PDS {
private:
    PCI_space *pci;
    USB_space *usb;
public:
    PDS() = default;
    void remove(unsigned long long);
};

#endif