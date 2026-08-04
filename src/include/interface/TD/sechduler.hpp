#ifndef __include_interface_TD_sechduler_hpp
#define __include_interface_TD_sechduler_hpp


#include <global/type.hpp>
#include "TCB.hpp"

class Sche {
public:
    virtual void init(TCB* tcbv, size_t tcvc) = 0;

    /**
     * Schedule Decision
     */
    virtual void ScheduleDecision() = 0;
    /**
     * TCB ID.
     * -1: this thread control block
     * -2: any thread control block
     */
    virtual void Switch(size_t to) = 0;
    virtual void cut(size_t thread_id, size_t int_num) = 0;
    virtual void run(void (*func)(size_t, void *), size_t argc, void *argv) = 0; // Create Thread

    virtual ~Sche() = default;
};

#endif