#include "SDM.hpp"

StreamDeviceManager *sdm = nullptr;

StreamDeviceManager::StreamDeviceManager(): space{}, id_count{1} {
    sdm = this;
}

void StreamDeviceManager::regist(Stream *stream, String name) {
    if (!stream) return;
    if (!name.empty()) {
        for (auto &e : space)
            if (e.name == name) return;   // 先到先得
    }
    const unsigned id = id_count++;
    space.append(Entry{stream, id, name});
}

void StreamDeviceManager::open(WinHandle &win, String name) {
    win.extra = nullptr;
    for (auto &e : space)
        if (e.name == name) {
            win.extra = e.stream;
            return;
        }
}

unsigned StreamDeviceManager::read(WinHandle &win, void *buf, unsigned n) {
    auto *stream = static_cast<Stream *>(win.extra);
    if (!stream || !buf || n == 0) return 0;

    auto *out = static_cast<u8 *>(buf);
    unsigned got = 0;
    while (got < n) {
        u8 b = 0;
        (*stream) >> b;
        if (b == 0) break;        // 没数据了，交给调用方决定等不等
        out[got++] = b;
    }
    return got;
}
