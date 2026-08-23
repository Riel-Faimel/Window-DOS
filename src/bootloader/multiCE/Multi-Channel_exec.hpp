#ifndef __bootloader_MCE_Multi_Channel_exec_hpp
#define __bootloader_MCE_Multi_Channel_exec_hpp

#include <global/type.hpp>

class multiCE {
public:
    multiCE();

    void run(
        void *func, size_t argc, void *argv,
        u8 ring = 3
    );
    void cut(size_t cpuid, struct TCB *tid, size_t intnum);
};

extern "C" {
    extern multiCE Multi_Channel_executor;
}

#endif