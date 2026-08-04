#ifndef __bootloader_multiCE_default_cs_hpp
#define __bootloader_multiCE_default_cs_hpp

#include <interface/TD/sechduler.hpp>
#include <TL/container>

class cs : public Sche {
    rtl::array<TCB> tcbv;
    size_t run_id;
public:
    cs();
    ~cs() override;
    void init(TCB*, size_t);

    void ScheduleDecision();
    void Switch(size_t to);
    void cut(size_t thread_id, size_t num);
    void run(void (*)(size_t, void *), size_t, void *); // Create Thread
};

#endif