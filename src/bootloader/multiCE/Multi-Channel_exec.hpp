#ifndef __bootloader_MCE_Multi_Channel_exec_hpp
#define __bootloader_MCE_Multi_Channel_exec_hpp

#include <global/type.hpp>
#include <interface/TD/sechduler.hpp>

struct _RZCTX;
class multiCE {
    Sche* scheduler;
    void (Sche::*switch_ptr)(size_t);

public:
    multiCE();

    void run(void (*func)(size_t, void *), size_t argc, void *argv);
    void cut(size_t, size_t);
    __attribute__((regparm(2)))
    void yield(_RZCTX *) __asm__ ("_RZmultiCE_yield");
};

extern "C" {
    extern multiCE* Multi_Channel_executor;
}

#endif