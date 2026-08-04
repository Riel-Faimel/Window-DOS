#include <global/type.hpp>
extern "C" void LoaderMain();

#pragma pack(push, 1)
struct BootINFO {
    unsigned char screen_mode: 1;
    unsigned char graphic_mode: 1;
    unsigned char buffer_base;
    unsigned char bpp;
    unsigned short width;
    unsigned short height;
    unsigned char boot_device;
    unsigned char mmap_size;
};
#pragma pack(pop)

extern bool *__boot_loader_end;

// bootloader modes
#include <mm/mm.hpp>
#include <DescrpTable/gdt.hpp>
#include <drv/drv.hpp>
#include <DescrpTable/idt.hpp>
#include <registry/registry.hpp>
#include <DescrpTable/IDNT.hpp>
#include <PDS/PCI/pci.hpp>
#include <SSS/SSS.hpp>
#include <TM/TM.hpp>
#include <interface/Window.hpp>

//#include <DOSsyscall/DOScall.hpp>

import changestack;
import lib32;
//import cs_mod;