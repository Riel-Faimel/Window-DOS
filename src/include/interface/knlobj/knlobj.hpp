#ifndef __include_interface_knlobj_knlobj_hpp
#define __include_interface_knlobj_knlobj_hpp

#include <string>
#include "diskinfo.hpp"

class KernelObj {
public:
    enum class TypeID {
        FramePort,
        LogicalDisk,
        SerialPort,
        PhysicalDisk,
    };
    virtual TypeID get_type() const = 0;
    virtual ~KernelObj() = default;
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
    TypeID get_type() const override { return TypeID::PhysicalDisk; }
    virtual unsigned cmd(unsigned, String, void *argv, unsigned argc) = 0;
    virtual ~PhysicalDisk() = default;
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
    TypeID get_type() const override { return TypeID::LogicalDisk; }
    virtual DiskInfo* info(unsigned = 0, String = {}) = 0;
    virtual unsigned cmd(unsigned, String, void *argv, unsigned argc) = 0;
    virtual ~LogicalDisk() = default;
};

class SerialPort : public KernelObj {
public:
    TypeID get_type() const override { return TypeID::SerialPort; }
    /**
     * take/give(buf, port, bytes)
     */
    virtual unsigned take(void *, unsigned, unsigned) = 0;
    virtual unsigned give(void *, unsigned, unsigned) = 0;
    virtual ~SerialPort() = default;
};

class FramePort : public KernelObj {
public:
    TypeID get_type() const override { return TypeID::FramePort; }
    /**
     * send/recv(buf, port, bytes)
     */
    virtual unsigned send(void *, unsigned, unsigned) = 0;
    virtual unsigned recv(void *, unsigned, unsigned) = 0;
    virtual ~FramePort() = default;
};

#endif