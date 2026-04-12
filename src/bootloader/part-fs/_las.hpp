#include "las.hpp"
#include "part-fs.hpp"
#define __independent_lib_Using_template_container
#include <TL/idlib/idlib>
#include <drv/PCI/pci.hpp>
#include <lib/cppstdlib/string.hpp>
#include <drv/disk/IDE/ide.hpp>
#include <drv/screen/screen_srv.hpp>

#include <CenterShell/cs.hpp>


inline void __dir(String str){
    if(str == ""){
        screen->print("Drive");screen->print("");screen->print("L");
        return;
    };
}