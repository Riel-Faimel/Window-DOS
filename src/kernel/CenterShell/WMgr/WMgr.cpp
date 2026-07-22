#include <interface/WMgr.hpp>
#include <interface/Window.hpp>

bool WinMgr::regist(WinHandle win){
    win.handle = NULL_PTR;
}
void WinMgr::destroy(WinHandle win){
    ;
}

WinMgr *WinHandleDOW_MANAGER;

WinHandle::WinHandle(){
    WinHandleDOW_MANAGER->regist(*this);
};
WinHandle::~WinHandle() noexcept{
    WinHandleDOW_MANAGER->destroy(*this);
}