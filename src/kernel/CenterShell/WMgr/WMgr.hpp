#define _BITS_64
#include <global/type.hpp>
#include <declar_/WIN.hpp>

class WinMgr{
private:
    friend class _WIN;
    bool regist(_WIN win);
    void destroy(_WIN win);
};

extern WinMgr *_WINDOW_MANAGER;