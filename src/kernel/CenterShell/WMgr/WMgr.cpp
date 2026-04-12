#include "WMgr.hpp"
#include <interpret/Window.hpp>

bool WinMgr::regist(_WIN win){
    win.handle = NULL_PTR;
}
void WinMgr::destroy(_WIN win){
    ;
}

WinMgr *_WINDOW_MANAGER;