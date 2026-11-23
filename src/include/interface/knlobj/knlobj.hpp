#ifndef __include_interface_knlobj_knlobj_hpp
#define __include_interface_knlobj_knlobj_hpp

#include <string>
#include "diskinfo.hpp"

// 内核对象基类。析构函数不内联，强制产生vtable
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
