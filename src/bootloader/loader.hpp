#define _BITS_32
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
#include <mm/gdt.hpp>
#include <drv/drv.hpp>
#include <srv/serve.hpp>
#include <IDT_L/idt.hpp>
#include <CenterShell/cs.hpp>
#include <registry/registry.hpp>
#include <IDT_L/IDNT.hpp>
#include <drv/PCI/pci.hpp>
#include <part-fs/las.hpp>
#include <DOSsyscall/DOScall.hpp>

#include <interface/Window.hpp>

//#include <hardlib/x86/port.h>
//#include <module/changestack.cpp>
import changestack;
import lib32;
//import cs_mod;