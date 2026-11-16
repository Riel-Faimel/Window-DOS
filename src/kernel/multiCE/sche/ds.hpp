#ifndef __kernel_multiCE_default_cs_hpp
#define __kernel_multiCE_default_cs_hpp

#include <interface/TD/TCB.hpp>
#include <interface/TD/context.hpp>

class DefaultScheduler {
    TCB *context_local = nullptr;
    TCB *uplist = nullptr;
    TCB *downlist = nullptr;
    TCB *sleeplist = nullptr;
    ArchContext &ctx;
    bool up_is_running = true; // else down is running

public:
    DefaultScheduler(ArchContext &ctx, TCB *initial);
    ~DefaultScheduler() {}

    TCB *current() const { return context_local; }

    void enqueue(TCB *tcb, u16 time);
    void ScheduleDecision();
    [[noreturn]] void resume();
    [[noreturn]] void handle_trap(unsigned code);

    void exit();
    void yield();
    void wait(TCB *tid);
    void wake(TCB *tid, size_t wake_num);
    void cut(TCB *tid, size_t num, void *argv);
    void set_time_size(TCB *tcb, u16 time);

    void reg_int(TCB *tcb, void *int_handler);
    void reg_wait(TCB *tcb, void *wait_handler);
};

#endif
