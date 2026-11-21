#ifndef __lib_lib32_hpp
#define __lib_lib32_hpp

/**
 * freestanding 运行时补的最小 C 库：
 * 端口 I/O 只是声明（定义在 lib/module/lib.cpp），memset/memcpy 是编译器会自己
 * 生成调用的符号，必须有真实定义。
 */
#include <global/type.hpp>
#include <port.h>

extern "C" {
    void *memset(void *ptr, int value, size_t num);
    void *memcpy(void *dst, const void *src, size_t num);
}

#endif
