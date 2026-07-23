#ifndef __include_interface_WMgr_hpp
#define __include_interface_WMgr_hpp

#define _BITS_64
#include <global/type.hpp>
#include <global/WIN_.hpp>

class WinMgr{
private:
    friend class WinHandle;
    bool regist(WinHandle win);
    void destroy(WinHandle win);
};

extern WinMgr *WinHandleDOW_MANAGER;

#endif