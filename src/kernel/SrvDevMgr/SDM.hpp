#ifndef __kernel_SrvDevMgr_SDM_hpp
#define __kernel_SrvDevMgr_SDM_hpp

#include <interface/srvdev/stream.hpp>
#include <String>
#include <interface/wm/Window.hpp>
#include <HL/container>

/**
 * Stream Device Manager：流服务的名字空间。
 *
 * 和 DLS 一个套路：驱动把流服务按名字注册进来，使用者按名字 open 出一个
 * 窗口句柄，之后从窗口句柄读数据。SDM 只做"名字 → 服务对象"的解析，
 * 不关心流是键盘、串口还是别的什么。
 */
class StreamDeviceManager {
public:
    struct Entry {
        Stream *stream;
        unsigned id;
        String name;
    };

    rtl::array<Entry> space;
    unsigned id_count = 1;

    StreamDeviceManager();

    /**
     * 注册一个流服务。名字重复会被拒绝（先到先得）。
     */
    void regist(Stream *, String = String{});

    /**
     * 按名字打开：把流服务挂到窗口句柄上（win.extra）；没找到就留空。
     */
    void open(WinHandle &, String);

    /**
     * 从窗口句柄读 n 个字节，返回实际读到的数量（0 = 现在没有数据）。
     */
    unsigned read(WinHandle &, void *buf, unsigned n);
};

extern StreamDeviceManager *sdm;

#endif
