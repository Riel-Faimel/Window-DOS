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