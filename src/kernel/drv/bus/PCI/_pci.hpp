#include "pci.hpp"
#include <global/type.hpp>
#undef LocalDebug
#include <global/macro.hpp>
#include <port.h>
#include <stdio>
#include <drv/kmod.hpp>
#include <PDS/pds.hpp>
#define PCI_address  0xCF8
#define PCI_data 0xCFC

enum {

StorageController = 0x01,
SCSIBus = 0x00,
IDE = 0x01,
IDEpi_ISAonly = 0x00,
IDEpi_PCIonly = 0x05,
IDEpi_ISAdefault = 0x0A,
FloppyDisk = 0x02,
RAID = 0x04,
ATA = 0x05,
SATA = 0x06,
SATApi_AHCI = 0x01,

NetworkController = 0x02,

DisplayController = 0x03,

MultiMediaController = 0x04,

MemoryController = 0x05,

Bridge = 0x06,

SerialBusController = 0x0C,

};