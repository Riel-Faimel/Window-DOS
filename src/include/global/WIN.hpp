#ifndef __globalWinHandle_hpp
#define __globalWinHandle_hpp
#include <global/type.hpp>
#include <interface/WMgr.hpp>

#pragma pack(push, 1)
class WinHandle {
public:
    friend class WinMgr;
    void *extra;
    unsigned long long handle;
    unsigned long long size;
    WinHandle():extra{nullptr}, handle{0}, size{0}{};
    WinHandle(WinHandle*);
    ~WinHandle(){};
};
#pragma pack(pop)

#endif