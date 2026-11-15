#ifndef __kernel_KOM_kom_hpp
#define __kernel_KOM_kom_hpp

#include <interface/knlobj/knlobj.hpp>
#include <interface/knlobj/converter.hpp>
#include <HL/container>

class KernelObjectManager {
    rtl::array<Converter *> conv_l;
public:
    KernelObjectManager();
    void append(Converter *);
    void append(KernelObj *);
};

extern KernelObjectManager *kom;

#endif