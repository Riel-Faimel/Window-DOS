#ifndef __include_interface_pift_hpp
#define __include_interface_pift_hpp
#include <global/type.hpp>

// Program Interface Index Table
enum class ProgIFIndex : u32 {
    begin = 0,

    PCI_IDE,
    PCI_Floppy,
    PCI_ATA_PI,
    PCI_AHCI,

    PCI_Display,

    PCI_SerialBus,
    ISA_PS2_KM,

    end,
};

#endif