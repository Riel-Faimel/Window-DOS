#ifndef __bootloader_MCE_Multi_Channel_exec_hpp
#define __bootloader_MCE_Multi_Channel_exec_hpp

#include <global/type.hpp>
#include <interface/TD/sechduler.hpp>

class multiCE {
    Sec* sechduler;

public:
    multiCE();

    void run(void (*func)(size_t, void *));
    void cut(size_t, size_t);
    void yield();
};

#endif