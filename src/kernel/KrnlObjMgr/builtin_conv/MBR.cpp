#include "_MBR.hpp"

Converter::MatchDegree builtin_converter::MBR::touch(KernelObj *kobj) {
    switch (kobj->get_type()) {
    case KernelObj::TypeID::PhysicalDisk: {
        auto phydisk = static_cast<PhysicalDisk *>(kobj);
        char buf[512];
        if (phydisk->read(buf, 0, 0, 1) != 512) return MatchDegree::Unknow;
        if ((u8)buf[510] == 0x55 && (u8)buf[511] == 0xAA) return MatchDegree::Match;
        return MatchDegree::Unknow;
    } break;
    default: {
        return MatchDegree::Unknow;
    } break;
    }
}

void builtin_converter::MBR::conv(KernelObj *kobj) {
    switch (kobj->get_type()) {
    case KernelObj::TypeID::PhysicalDisk: {
        auto disk = static_cast<PhysicalDisk *>(kobj);
        mbr buf;
        if (disk->read(buf.buf, 0, 0, 1) != 512)
            return;
        if (buf.sign != 0xAA55) return;

        for (auto &entry : buf.part) {
            if (entry.system_id == 0) continue;

            u32 start = entry.start_LBA_low | (entry.start_LBA_high << 16);
            u32 size = entry.sector_count_low | (entry.sector_count_high << 16);
            if (size == 0) continue; // not a part
            auto newpart = new SingalPart{
                disk, (u64)start, (u64)size, entry.system_id
            };
            kom->append(newpart);
        }
    } break;
    default: {
        return;
    } break;
    }
}