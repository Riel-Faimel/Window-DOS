#ifndef __kernel_KOM_builtin_conv_MBR_hpp
#define __kernel_KOM_builtin_conv_MBR_hpp

#include "part.hpp"
#include <interface/knlobj/converter.hpp>

namespace builtin_converter{
        
class MBR : public Converter {
public:
    MatchDegree touch(KernelObj *kobj) override {
        switch (kobj->get_type()) {
        case KernelObj::TypeID::PhysicalDisk: {
            auto phydisk = static_cast<PhysicalDisk *>(kobj);
            char buf[512];
            if (phydisk->read(buf, 0, 0, 1) != 512)
                return MatchDegree::Unknow;

            if ((u8)buf[510] == 0x55 && (u8)buf[511] == 0xAA)
                return MatchDegree::Match;
            return MatchDegree::Unknow;
        } break;
        default: {
            return MatchDegree::Unknow;
        } break;
        }
    }

    void conv(KernelObj *kobj) override {
        switch (kobj->get_type()) {
        case KernelObj::TypeID::PhysicalDisk: {
            auto disk = static_cast<PhysicalDisk *>(kobj);
            char buf[512];
            if (disk->read(buf, 0, 0, 1) != 512)
                return;
            if ((u8)buf[510] != 0x55 || (u8)buf[511] != 0xAA)
                return;

            for (unsigned i = 0; i < 4; i++) {
                unsigned off = 446 + i * 16;
                u8 type = (u8)buf[off + 4];
                u32 size =
                    (u32)(u8)buf[off + 12] |
                    ((u32)(u8)buf[off + 13] << 8) |
                    ((u32)(u8)buf[off + 14] << 16) |
                    ((u32)(u8)buf[off + 15] << 24);
                if (type == 0 || size == 0) continue;

                u32 start =
                    (u32)(u8)buf[off + 8] |
                    ((u32)(u8)buf[off + 9] << 8) |
                    ((u32)(u8)buf[off + 10] << 16) |
                    ((u32)(u8)buf[off + 11] << 24);

                auto part = new SingalPart{disk, (u64)start, (u64)size, type};
                kom->append(part);
            }
        } break;
        default: {
            return;
        } break;
        }
    }
};

} // namespace builtin_converter

#endif
