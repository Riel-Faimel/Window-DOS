#include "pci.hpp"
#define _BITS_32
#include <global/type.hpp>
#include <cstdlib/port.h>
#include <drv/screen/screen_srv.hpp>
#include <drv/disk/IDE/ide_controller_init.hpp>
#include <lib/runenv.hpp>
#define PCI_address  0xCF8
#define PCI_data 0xCFC
