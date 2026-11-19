#ifndef __kernel_drv_disk_ATA_ATAdef_hpp
#define __kernel_drv_disk_ATA_ATAdef_hpp

#include <global/type.hpp>
#include <global/cluster.hpp>

namespace ATAdev {

using clu::CHS;
using clu::LBA;
using clu::SectorCount;
    
class byteCount {
    size_t offset;
public:
    byteCount() : offset{0} {}
    explicit byteCount(size_t off) : offset{off} {}
    explicit byteCount (SectorCount count): offset{count.value() * 512} {}

    size_t value() const { return offset; }
    size_t to_size_t() const { return offset; }
};
class DeviceIndex {
    u8 data;
public:
    DeviceIndex() : data{0} {}
    explicit DeviceIndex(u8 id) : data{id} {}

    explicit operator u8 () const { return data; }
};
class bufAddr {
    void* buf;
public:
    bufAddr() : buf{nullptr} {}
    explicit bufAddr(void* p) : buf{p} {}

    explicit operator void *() { return buf; }
    u16 &operator [] (size_t index) { return static_cast<u16 *>(buf)[index]; }
};
    
} // namespace ATAdef 

#endif