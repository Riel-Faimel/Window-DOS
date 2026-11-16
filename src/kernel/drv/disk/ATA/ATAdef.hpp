#ifndef __kernel_drv_disk_ATA_ATAdef_hpp
#define __kernel_drv_disk_ATA_ATAdef_hpp

#include <global/type.hpp>

namespace ATAdev {

struct LBAaddr : u64 {
    LBAaddr() : u64{} {}
    // One argument is the low 32 bits of the LBA.
    explicit LBAaddr(u32 low) : u64{0, low} {}
    // Two arguments are (high, low), matching the u64 layout.
    LBAaddr(u32 high, u32 low) : u64{high, low} {}
};
class SectorCount {
    u16 count;
public:
    SectorCount (u32) = delete;
    explicit SectorCount(): count{0} {}
    explicit SectorCount(u16 count_) : count{count_} {}
    explicit operator u16 () { return static_cast<u16>(count); }

    SectorCount &operator++ () { ++count; return *this; }
    bool operator!= (const SectorCount &oth) const { return count != oth.count; }

    size_t value() const { return count; }
};
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