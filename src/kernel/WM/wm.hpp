#ifndef __include_interface_WMgr_hpp
#define __include_interface_WMgr_hpp

#include <global/type.hpp>
#include <interface/syscall.hpp>
#include <HL/container>

class WinHandle;

class WindowManager {
public:
    WindowManager();

    bool regist(WinHandle &win);
    bool destroy(WinHandle &win);

    bool append(SyscallHandler *strategy, unsigned vector);

private:
    struct SyscallEntry {
        SyscallHandler *strategy;
        unsigned vector;
    };

    rtl::list<WinHandle *> handles;
    rtl::list<SyscallEntry> strategies;
    u32 next_handle = 1;
};

extern WindowManager *window_manager;

#endif
