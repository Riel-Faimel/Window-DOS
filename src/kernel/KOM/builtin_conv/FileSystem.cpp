#include "_FileSystem.hpp"

namespace {
    
void PartInfoPrint(u8 sysid) {
    kout << "[FS] +";
    switch (sysid) {
    case 0x0E:
        kout << "FAT16";
        break;
    default:
        kout << "RAW";
        break;
    }
}

} // namespace 


Converter::MatchDegree builtin_converter::FileSystem::touch(KernelObj *kobj) {
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

void builtin_converter::FileSystem::conv(KernelObj *kobj) {
    switch (kobj->get_type()) {
    case KernelObj::TypeID::LogicalDisk: {
        auto disk = static_cast<LogicalDisk *>(kobj);
        auto info = disk->info();
        if (!info || info->type == 0) return;
        Cluster *fs;
        if (kmod->fs.exists(info->type)) {
            PartInfoPrint(info->type);
            fs = kmod->fs[info->type](disk);
        }
        else fs = new RAW{disk};
        driverletterspace->regist(fs);
    } break;
    default: {
        return;
    } break;
    }
}