#ifndef __include_interface_pift_hpp
#define __include_interface_pift_hpp
#include <global/type.hpp>

// Program Interface Index Table
enum class ProgIFIndex : u32 {
    begin = 0,
    PCI_IDE,
    PCI_AHCI,
    end,
};

#endif