#ifndef __globalWinHandle_hpp
#define __globalWinHandle_hpp
#include <global/type.hpp>
#include <interface/WMgr.hpp>

#pragma pack(push, 1)
class WinHandle {
protected:
public:
    friend class WinMgr;
    unsigned long long handle;
    unsigned long long flag;
    void *extra;
    unsigned long long size;
    inline WinHandle(){};
    WinHandle(WinHandle*);
    inline ~WinHandle(){};
};
#pragma pack(pop)

#endif