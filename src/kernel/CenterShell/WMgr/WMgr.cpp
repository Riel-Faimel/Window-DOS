#include "WMgr.hpp"
#include <interface/Window.hpp>

bool WinMgr::regist(_WIN win){
    win.handle = NULL_PTR;
}
void WinMgr::destroy(_WIN win){
    ;
}

WinMgr *_WINDOW_MANAGER;