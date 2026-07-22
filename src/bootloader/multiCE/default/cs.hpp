#ifndef __bootloader_multiCE_default_cs_hpp
#define __bootloader_multiCE_default_cs_hpp

#include <interface/TD/sechduler.hpp>

class cs : public Sec {
    TCB *tcbv;
    size_t capacity;
    size_t run_id;
public:
    ~cs() override;
    void init(TCB*);

    void Switch() __asm__("_ScsSwitch");

    void kill(); // Run/Wait -> Die
    void yield(); // Run -> Wait
    void Ready(); // Wait -> Run

    void cut(size_t thread_id, size_t num);

    void run(void (*)(size_t, void *)); // Create Thread
};

#endif