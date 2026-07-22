#ifndef __include_interface_TD_sechduler_hpp
#define __include_interface_TD_sechduler_hpp


#include <global/type.hpp>
#include "TCB.hpp"

class Sec {
public:
    virtual void init(TCB*) = 0;

    virtual void Switch() = 0;

    virtual void kill() = 0; // Run/Wait -> Die
    virtual void yield() = 0; // Run -> Wait
    virtual void Ready() = 0; // Wait -> Run

    virtual void cut(size_t, size_t) = 0;

    virtual void run(void (*)(size_t, void *)) = 0; // Create Thread

    virtual ~Sec() = default;
};

#endif