#ifndef ____Window
#define ____Window
#include <CenterShell/WMgr/WMgr.hpp>

#pragma pack(push, 1)
class _WIN {
protected:
    friend class WinMgr;
    unsigned long long handle;
    unsigned long long flag;
    void *extra;
    unsigned long long size;
public:
    _WIN();
    ~_WIN();
};
#pragma pack(pop)

inline _WIN::_WIN(){
    _WINDOW_MANAGER->regist(*this);
};
inline _WIN::~_WIN() noexcept{
    _WINDOW_MANAGER->destroy(*this);
}

#endif