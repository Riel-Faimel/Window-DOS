#include <interface/WMgr.hpp>
#include <interface/Window.hpp>

bool WinMgr::regist(_WIN win){
    win.handle = NULL_PTR;
}
void WinMgr::destroy(_WIN win){
    ;
}

WinMgr *_WINDOW_MANAGER;

_WIN::_WIN(){
    _WINDOW_MANAGER->regist(*this);
};
_WIN::~_WIN() noexcept{
    _WINDOW_MANAGER->destroy(*this);
}