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
#include <SrvDevMgr/cdm.hpp>
#include <SrvDevMgr/SDM.hpp>
#include <KrnlObjMgr/kom.hpp>
#include <TaskMgr/TM.hpp>
#include <drv/kmod.hpp>
#include <svm/svm.hpp>
#include <basicshell/bs.hpp>
#include <WM/wm.hpp>
#include <syscall/thread>
#include <drv/screen/screen_srv.hpp>
#ifdef COMDebug
#include <drv/com/com.hpp>
#endif