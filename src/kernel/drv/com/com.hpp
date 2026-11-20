#ifndef __kernel_drv_com_com_hpp
#define __kernel_drv_com_com_hpp

#include <stdio>

/**
 * COM1 输出后端。
 *
 * 只在定义 COMDebug 时被 kout 使用，用来在没有屏幕的场合（自动化、
 * 远程）拿内核输出。构造时按 115200 8N1 初始化 8250。
 */
class com_output : public OBackend {
public:
    com_output();

    void write(const char *buf, size_t len) override;
};

extern com_output comout;

#endif
