#ifndef __kernel_KOM_builtin_conv_FileSystem_hpp
#define __kernel_KOM_builtin_conv_FileSystem_hpp

#include "part.hpp"
#include <interface/knlobj/converter.hpp>

namespace {
    
void PartInfoPrint(u8 sysid) {
    kout << "[PM] +";
    switch (sysid) {
    case 0x0E:
        kout << "FAT16";
        break;
    default:
        kout << "RAW";
        break;
    }
    kout << " Part, ";
}

} // namespace 

namespace builtin_converter {

class FileSystem : public Converter {
public:
    MatchDegree touch(KernelObj *kobj) override {
        switch (kobj->get_type()) {
        case KernelObj::TypeID::LogicalDisk: {
            auto disk = static_cast<LogicalDisk *>(kobj);
            auto info = disk->info();
            if (!info || info->type == 0)
                return MatchDegree::Unknow;

            if (kmod->fs.exists(info->type))
                return MatchDegree::Match;
            return MatchDegree::Work;
        } break;
        default: {
            return MatchDegree::Unknow;
        } break;
        }
    }

    void conv(KernelObj *kobj) override {
        switch (kobj->get_type()) {
        case KernelObj::TypeID::LogicalDisk: {
            auto disk = static_cast<LogicalDisk *>(kobj);
            auto info = disk->info();
            if (!info || info->type == 0)
                return;

            Cluster *fs;
            if (kmod->fs.exists(info->type))
                fs = kmod->fs[info->type](disk);
            else
                fs = new RAW{disk};
            driverletterspace->regist(fs);
        } break;
        default: {
            return;
        } break;
        }
    }
};

} // namespace builtin_converter

#endif
