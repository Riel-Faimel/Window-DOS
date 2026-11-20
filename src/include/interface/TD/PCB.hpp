#ifndef __include_TD_PCB_hpp
#define __include_TD_PCB_hpp

#include <global/type.hpp>
#include <String>
#include <interface/mem/allocable.hpp>
#include "TCB.hpp"

struct PCB {
    TCB *main;
    mem::MemSpace ms;
    String name;
};


#endif