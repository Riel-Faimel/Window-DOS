#include <global/type.hpp>
extern "C" {
    void LoaderMain();
}

#define ArchitectureDetail

// kernel modes
#include <global/macro.hpp>
#include <arch/start.hpp>
#include <arch/cpu.hpp>
#include <id/id.hpp>
#include <mm/mm.hpp>
#include <PDS/pds.hpp>
#include <CDM/cdm.hpp>
#include <KOM/kom.hpp>
#include <TM/TM.hpp>
#include <drv/kmod.hpp>
#include <basicshell/bs.hpp>
#include <WM/wm.hpp>