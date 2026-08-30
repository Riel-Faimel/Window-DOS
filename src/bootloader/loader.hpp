#include <global/type.hpp>
extern "C" {
    void LoaderMain();
}


// bootloader modes
#include <mm/mm.hpp>
#include <multiCE/cpu.hpp>
#include <drv/drv.hpp>
#include <DescripTable/intdis.hpp>
#include <registry/registry.hpp>
#include <DescripTable/IDNT.hpp>
#include <PDS/PCI/pci.hpp>
#include <SSS/SSS.hpp>
#include <TM/TM.hpp>
#include <interface/Window.hpp>
#include <start/BIOSinfo.hpp>
#include <kmod/kmod.hpp>

//#include <DOSsyscall/DOScall.hpp>

import changestack;
import lib32;
//import cs_mod;