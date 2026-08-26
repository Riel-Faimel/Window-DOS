#ifndef __bootloader_MCE_Multi_Channel_exec_hpp
#define __bootloader_MCE_Multi_Channel_exec_hpp

#include <global/type.hpp>

class multiCE {
public:
    multiCE();

    void run(
        void (*)(size_t, void *), size_t, void *, size_t time = 1, u8 ring = 3,
        size_t cs = 0, size_t ds = 0, size_t gs = 0, size_t fs = 0
    );
    void cut(size_t cpuid, struct TCB *tid, size_t intnum);
};

extern "C" {
    extern multiCE Multi_Channel_executor;
}

#endif