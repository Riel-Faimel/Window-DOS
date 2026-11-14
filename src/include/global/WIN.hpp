#ifndef __globalWinHandle_hpp
#define __globalWinHandle_hpp
#include <global/type.hpp>
#include <interface/WMgr.hpp>

#pragma pack(push, 1)
class WinHandle {
public:
    friend class WindowManager;
    void *extra;
    unsigned long long handle;
    unsigned long long size;

    WinHandle(): extra{nullptr}, handle{0}, size{0} {
        if (window_manager) window_manager->regist(*this);
    }

    WinHandle(WinHandle *oth): extra{oth ? oth->extra : nullptr}, handle{0}, size{oth ? oth->size : 0} {
        if (window_manager) window_manager->regist(*this);
    }

    WinHandle(const WinHandle &oth): extra{oth.extra}, handle{0}, size{oth.size} {
        if (window_manager) window_manager->regist(*this);
    }

    WinHandle &operator = (const WinHandle &oth) {
        if (this == &oth) return *this;
        if (window_manager && handle) window_manager->destroy(*this);
        extra = oth.extra;
        size = oth.size;
        handle = 0;
        if (window_manager) window_manager->regist(*this);
        return *this;
    }

    ~WinHandle() {
        if (window_manager && handle) window_manager->destroy(*this);
    }
};
#pragma pack(pop)

#endif