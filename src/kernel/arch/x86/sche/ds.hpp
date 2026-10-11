#ifndef __kernel_multiCE_default_cs_hpp
#define __kernel_multiCE_default_cs_hpp

#include <interface/TD/sechduler.hpp>
#include "..\cpu\idt.hpp"

class MemoryManager;

extern "C" {
    void _Scall_yield();
    void _Sint_sche();
    void _Sint_yield();
    void _Sexit();
}

namespace arch {

class DefaultScheduler {
    TCB *context_local = nullptr;
    TCB *uplist = nullptr;
    TCB *downlist = nullptr;
    TCB *sleeplist = nullptr;
    MemoryManager& mm;
    bool up_is_running = true; // else down is running
public:
    DefaultScheduler(MemoryManager&, arch::IDT&);
    ~DefaultScheduler(){}

    inline void ScheduleDecision();
    [[noreturn]] void resume();
    __attribute__((optimize("O3"), used))
    inline void run(
        void *func, size_t argc, void *argv, size_t time, 
        size_t code_seg, size_t data_seg, size_t gs, size_t fs
    ); // Create Thread
    inline void exit();
    inline void join(TCB *);
    inline void yield();
    inline void wait(TCB *);
    inline void wake(TCB *, size_t);
    inline void cut(TCB *, size_t, void *);
    inline void set_time_size(TCB *, u16);

    inline void reg_int(TCB *, void *);
    inline void reg_wait(TCB *, void *);

    void start_preemption(arch::IDT &);
    void close_preemption(arch::IDT &);
};
    
} // namespace arch 

#endif