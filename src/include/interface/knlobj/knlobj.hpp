#ifndef __include_interface_knlobj_knlobj_hpp
#define __include_interface_knlobj_knlobj_hpp

#include <string>
#include "diskinfo.hpp"

/**
 * 内核对象基类。
 *
 * 类型识别不再用 get_type()/TypeID：需要区分类型的地方一律 dynamic_cast，
 * 这样"有哪些内核对象类型"不需要一个中心枚举，加一个类型不用改基类。
 *
 * 析构函数故意不在头里 inline：它是这几个多态基类的 key function，vtable 的
 * 发源点因此收敛到 knlobj_exp.cpp（那个 TU 用 -frtti 编），dynamic_cast 才有
 * typeinfo 可读。
 */
class KernelObj {
public:
    virtual ~KernelObj();
};

class LBAop {
public:
    /**
     * read/write(buffer, LBA, byte_skip, sector_deal)
     */
    virtual unsigned read (void *, unsigned, unsigned, unsigned) = 0;
    virtual unsigned write(void *, unsigned, unsigned, unsigned) = 0;
};

/**
 * real disk and virtual disk also logical disk
 */
class PhysicalDisk : public KernelObj, public LBAop {
public:
    virtual unsigned cmd(unsigned, String, void *argv, unsigned argc) = 0;
    ~PhysicalDisk() override;
};

/**
 * real disk and virtual disk also logical disk
 */
class LogicalDisk : public KernelObj, public LBAop {
public:
    /**
     * if disk driver realize 
     * read/write(buffer, LBA, byte_offset, sector_deal)
     * 
     * if File system realize
     * read/write(buffer, handle, byte_offset, byte_deal)
     */
    virtual DiskInfo* info(unsigned = 0, String = {}) = 0;
    virtual unsigned cmd(unsigned, String, void *argv, unsigned argc) = 0;
    ~LogicalDisk() override;
};

class SerialPort : public KernelObj {
public:
    /**
     * take/give(buf, port, bytes)
     */
    virtual unsigned take(void *, unsigned, unsigned) = 0;
    virtual unsigned give(void *, unsigned, unsigned) = 0;
    ~SerialPort() override;
};

class FramePort : public KernelObj {
public:
    /**
     * send/recv(buf, port, bytes)
     */
    virtual unsigned send(void *, unsigned, unsigned) = 0;
    virtual unsigned recv(void *, unsigned, unsigned) = 0;
    ~FramePort() override;
};

#endif
