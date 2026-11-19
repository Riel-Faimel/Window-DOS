#pragma once

#include "type.hpp"

namespace clu {

class LBA;

class CHS {
public:
    u16 cylinder = 0;
    u8 head = 0;
    u8 sector = 0;
    u16 sectors_per_track = 0;
    u8 heads = 0;

    CHS() = default;
    constexpr CHS(u16 cyl, u8 head_, u8 sect, u16 spt = 0, u8 heads_ = 0):
    cylinder{cyl}, head{head_}, sector{sect},
    sectors_per_track{spt}, heads{heads_} {}

    constexpr CHS(LBA lba, u16 spt, u8 heads_);
};

class LBA {
    u64 addr;

public:
    constexpr LBA() : addr{} {}
    constexpr explicit LBA(u32 low) : addr{0, low} {}
    constexpr LBA(u32 high, u32 low) : addr{high, low} {}
    constexpr LBA(CHS chs): addr{
        0, (static_cast<u32>(chs.cylinder) * chs.heads + chs.head) * 
        chs.sectors_per_track + (chs.sector ? chs.sector - 1u : 0u)
    } {}

    constexpr u32 low() const { return addr.low; }
    constexpr u32 high() const { return addr.high; }

    LBA &operator += (u32 sectors) {
        addr += sectors;
        return *this;
    }
};

constexpr CHS::CHS(LBA lba, u16 spt, u8 heads_):
sectors_per_track{spt}, heads{heads_} {
    if (!spt || !heads_) return;

    const u32 per_cyl = static_cast<u32>(spt) * heads_;
    cylinder = static_cast<u16>(lba.low() / per_cyl);

    const u32 rest = lba.low() % per_cyl;
    head = static_cast<u8>(rest / spt);
    sector = static_cast<u8>(rest % spt + 1);
}

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


} // namespace clu 
