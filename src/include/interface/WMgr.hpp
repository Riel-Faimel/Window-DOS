#ifndef __include_interface_WMgr_hpp
#define __include_interface_WMgr_hpp

#include <global/type.hpp>
class WinHandle;

class WinMgr{
private:
    friend class WinHandle;
    bool regist(WinHandle win);
    void destroy(WinHandle win);
};

extern WinMgr *WinHandleDOW_MANAGER;

#endif