#ifndef __globalWinHandle_hpp
#define __globalWinHandle_hpp
#include <global/type.hpp>

#pragma pack(push, 1)
class WinHandle {
    u32 handle;
    friend class WindowManager;
public:
    void *extra;
    u32 size;

    WinHandle();
    WinHandle(WinHandle *oth);
    WinHandle(const WinHandle &oth);
    WinHandle &operator = (const WinHandle &oth);
    ~WinHandle();
};
#pragma pack(pop)

#endif