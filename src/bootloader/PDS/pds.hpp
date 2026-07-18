#ifndef __bootloader_PDS_pds_hpp
#define __bootloader_PDS_pds_hpp

/**
 * PDS (Physical Driver Space) is a space for all physical drivers
 * it is manage enumerator objects and provide driver queries and API for caller 
 */

#include <interface/dev_cls/logicaldisk.hpp>
#include <TL/container>

#include <PDS/PCI/pci.hpp>
#include <PDS/USB/usb.hpp>

class PDS {
private:
    PCI_space pci;
    USB_space usb;

    rtl::map<size_t, void *> drivers;
public:
    PDS();
    void probe();
    void set_driver();
    void remove(unsigned long long);
};

#endif