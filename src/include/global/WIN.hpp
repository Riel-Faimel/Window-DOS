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
    WinHandle():handle{0}, flag{0}, extra{nullptr}, size{0}{};
    WinHandle(WinHandle*);
    ~WinHandle(){};
};
#pragma pack(pop)

#endif