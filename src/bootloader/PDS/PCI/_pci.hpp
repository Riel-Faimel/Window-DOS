#include "pci.hpp"
#include <global/type.hpp>
#undef LocalDebug
#include <global/macro.hpp>
#include <port.h>
#include <drv/screen/screen_srv.hpp>
#include <drv/kmod.hpp>
#define PCI_address  0xCF8
#define PCI_data 0xCFC