#include <global/type.hpp>
extern "C" {
    void LoaderMain();
}


// kernel modes
#include <mm/mm.hpp>
#include <arch/cpu.hpp>
#include <drv/drv.hpp>
#include <id/id.hpp>
#include <PDS/PCI/pci.hpp>
#include <CDM/cdm.hpp>
#include <TM/TM.hpp>
#include <interface/Window.hpp>
#include <arch/start.hpp>
#include <drv/kmod.hpp>
#include <global/macro.hpp>

//#include <DOSsyscall/DOScall.hpp>

import changestack;
import lib32;
//import cs_mod;