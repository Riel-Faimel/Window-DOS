#include "wm.hpp"

#include <global/WIN.hpp>
#include <id/id.hpp>

WindowManager *window_manager __attribute__((section(".data"))) = nullptr;

namespace {

struct Slot {
    void *extra;
    unsigned long long handle;
    unsigned long long size;
};

__attribute__((section(".data"))) Slot slots[256]{};

} // namespace

bool WindowManager::regist(WinHandle &win) {
    if (win.handle) return true;

    for (unsigned i = 0; i < 256; ++i) {
        if (slots[i].handle) continue;

        slots[i].extra = win.extra;
        slots[i].size = win.size;
        slots[i].handle = i + 1;
        win.handle = i + 1;
        return true;
    }
    return false;
}

bool WindowManager::destroy(WinHandle &win) {
    if (!win.handle) return false;

    auto id = win.handle;
    win.handle = 0;

    if (id == 0 || id > 256) return false;
    if (slots[id - 1].handle != id) return false;

    slots[id - 1] = Slot{};
    return true;
}

WindowManager::WindowManager() {
    window_manager = this;
}

bool WindowManager::append(SyscallHandler *strategy, unsigned vector) {
    if (!strategy || count >= 8) return false;

    strategies[count++] = Entry{strategy, vector};
    interrupt_distributor.reg_int(strategy->handler(), vector);
    return true;
}

void WindowManager::install_on(arch::CPU &cpu) {
    for (unsigned i = 0; i < count; ++i) {
        cpu.regist_int_handler(
            strategies[i].strategy->handler(),
            strategies[i].vector,
            arch::CPU::AccessControl::Public
        );
    }
}
