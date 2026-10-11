#include "pci.hpp"
#include <global/type.hpp>
#undef LocalDebug
#include <global/macro.hpp>
#include <port.h>
#include <drv/screen/screen_srv.hpp>
#include <drv/kmod.hpp>
#include <PDS/pds.hpp>
#define PCI_address  0xCF8
#define PCI_data 0xCFC


#define StorageController 0x01
#define SCSIBus 0x00
#define IDE 0x01
#define IDEpi_ISAonly 0x00
#define IDEpi_PCIonly 0x05
#define IDEpi_ISAdefault 0x0A
#define FloppyDisk 0x02
#define RAID 0x04
#define ATA 0x05
#define SATA 0x06
#define SATApi_AHCI 0x01

#define NetworkController 0x02
#define DisplayController 0x03
#define MultiMediaController 0x04
#define MemoryController 0x05
#define Bridge 0x06
#define SerialBusController 0x0C
