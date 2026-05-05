#ifndef __include_interface_WMgr_hpp
#define __include_interface_WMgr_hpp

#define _BITS_64
#include <global/type.hpp>
#include <global/WIN_.hpp>

class WinMgr{
private:
    friend class _WIN;
    bool regist(_WIN win);
    void destroy(_WIN win);
};

extern WinMgr *_WINDOW_MANAGER;

#endif