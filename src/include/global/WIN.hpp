#ifndef __global_WIN_hpp
#define __global_WIN_hpp
#include <global/type.hpp>
#include <interface/WMgr.hpp>

#pragma pack(push, 1)
class _WIN {
protected:
public:
    friend class WinMgr;
    unsigned long long handle;
    unsigned long long flag;
    void *extra;
    unsigned long long size;
    inline _WIN(){};
    _WIN(_WIN*);
    inline ~_WIN(){};
};
#pragma pack(pop)

#endif