#include <global/type.hpp>
extern "C" {
    void LoaderMain();
}


// bootloader modes
#include <mm/mm.hpp>
#include <arch/cpu.hpp>
#include <drv/drv.hpp>
#include <id/id.hpp>
#include <registry/registry.hpp>
#include <PDS/PCI/pci.hpp>
#include <CDM/cdm.hpp>
#include <TM/TM.hpp>
#include <interface/Window.hpp>
#include <start/BIOSinfo.hpp>
#include <drv/kmod.hpp>
#include <global/macro.hpp>

//#include <DOSsyscall/DOScall.hpp>

import changestack;
import lib32;
//import cs_mod;