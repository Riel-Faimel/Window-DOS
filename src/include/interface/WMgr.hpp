#ifndef __include_interface_WMgr_hpp
#define __include_interface_WMgr_hpp

#include <global/type.hpp>
#include <interface/syscall.hpp>

class WinHandle;
namespace arch { class CPU; }

class WindowManager {
public:
    WindowManager();

    bool regist(WinHandle &win);
    bool destroy(WinHandle &win);

    bool append(SyscallHandler *strategy, unsigned vector);
    void install_on(arch::CPU &cpu);

private:
    struct Entry {
        SyscallHandler *strategy;
        unsigned vector;
    };

    Entry strategies[8]{};
    unsigned count = 0;
};

extern WindowManager *window_manager;

#endif
