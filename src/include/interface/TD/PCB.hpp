#ifndef __include_TD_PCB_hpp
#define __include_TD_PCB_hpp

#include <global/type.hpp>
#include <String>

struct MemSpace;
struct TCB;

struct PCB {
    TCB *main;
    MemSpace &ms;
    String name;
};


#endif