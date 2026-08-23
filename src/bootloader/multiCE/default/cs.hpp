#ifndef __bootloader_multiCE_default_cs_hpp
#define __bootloader_multiCE_default_cs_hpp

#include <interface/TD/sechduler.hpp>
#include <TL/container>

extern "C" {
    void _Syield();
    void _Ssche();
    void _Sexit();
}

class DefaultScheduler{
    TCB *context_local = nullptr;
    TCB *uplist = nullptr;
    TCB *downlist = nullptr;
    TCB *sleeplist = nullptr;
    bool up_is_running = true; // else down is running
public:
    DefaultScheduler();
    ~DefaultScheduler(){}

    inline void ScheduleDecision();
    [[noreturn]] void resume();
    inline void yield();
    inline void exit(TCB * = nullptr);
    //inline void Switch();
    void cut(TCB *thread_id, size_t num);
    void run(void *func, size_t argc, void *argv, u8 ring, size_t time = 1); // Create Thread
};

#endif