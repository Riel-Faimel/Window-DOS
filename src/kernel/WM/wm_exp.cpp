#include "_wm.hpp"

WindowManager *window_manager __attribute__((section(".data"))) = nullptr;

WinHandle::WinHandle():handle{0},  extra{nullptr}, size{0} {
    if (!window_manager || !window_manager->regist(*this)) [[unlikely]]
        throw "WinHandle registration failed";
}

WinHandle::WinHandle(WinHandle *oth):
handle{0}, extra{oth ? oth->extra : nullptr}, size{oth ? oth->size : 0} {
    if (!window_manager || !window_manager->regist(*this)) [[unlikely]]
        throw "WinHandle registration failed";
}

WinHandle::WinHandle(const WinHandle &oth):
handle{0}, extra{oth.extra}, size{oth.size} {
    if (!window_manager || !window_manager->regist(*this)) [[unlikely]]
        throw "WinHandle registration failed";
}

WinHandle &WinHandle::operator=(const WinHandle &oth) {
    if (this == &oth) return *this;

    if (handle && window_manager)
        window_manager->destroy(*this);

    extra = oth.extra;
    size = oth.size;
    handle = 0;

    if (!window_manager || !window_manager->regist(*this)) [[unlikely]]
        throw "WinHandle registration failed";

    return *this;
}

WinHandle::~WinHandle() {
    if (handle && window_manager)
        window_manager->destroy(*this);
}

bool WindowManager::regist(WinHandle &win) {
    if (win.handle) return true;
    if (next_handle == 0) return false;

    handles.append(&win);
    win.handle = next_handle++;
    return true;
}

bool WindowManager::destroy(WinHandle &win) {
    if (!win.handle) return false;

    for (size_t i = 0; i < handles.get_size(); i++) {
        if (handles[i] != &win) continue;

        handles.remove(i);
        win.handle = 0;
        return true;
    }
    return false;
}

WindowManager::WindowManager(): handles{}, strategies{} {
    if (window_manager) [[unlikely]]
        throw "WindowManager already initialized";

    window_manager = this;
}

bool WindowManager::append(SyscallHandler *strategy, unsigned vector) {
    if (!strategy) return false;

    strategies.append(SyscallEntry{strategy, vector});
    interrupt_distributor.reg_int(strategy->handler(), vector);
    return true;
}
