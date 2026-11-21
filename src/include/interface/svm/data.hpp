#pragma once

#include <global/type.hpp>

namespace svm {

struct OutBuf {
    u8 *data = nullptr;
    unsigned cap = 0;
    unsigned len = 0;

    bool put(const void *src, unsigned n) {
        if (!data || n > cap - len) return false;
        auto *p = static_cast<const u8 *>(src);
        for (unsigned i = 0; i < n; i++) data[len + i] = p[i];
        len += n;
        return true;
    }

    template<typename T>
    bool put_as(const T &v) { return put(&v, sizeof(T)); }

    void reset() { len = 0; }
};
    
} // namespace svm 
