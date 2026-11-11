#include <global/type.hpp>
extern "C" {
    void LoaderMain();
}


// kernel modes
#include <global/macro.hpp>

#include <arch/start.hpp>
#include <arch/cpu.hpp>
#include <id/id.hpp>
#include <mm/mm.hpp>
#include <drv/drv.hpp>
#include <PDS/pds.hpp>
#include <CDM/cdm.hpp>
#include <TM/TM.hpp>
#include <drv/kmod.hpp>

#include <interface/Window.hpp>

//#include <DOSsyscall/DOScall.hpp>

import changestack;
import lib32;
//import cs_mod;