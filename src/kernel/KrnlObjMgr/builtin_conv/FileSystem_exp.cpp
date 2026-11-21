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
    kout.ch('\n');
}

} // namespace 


Converter::MatchDegree builtin_converter::FileSystem::touch(KernelObj *kobj) {
    auto *disk = dynamic_cast<LogicalDisk *>(kobj);
    if (!disk) return MatchDegree::Unknow;

    auto info = disk->info();
    if (!info || info->type == 0)
        return MatchDegree::Unknow;

    if (kmod->fs.exists(info->type))
        return MatchDegree::Match;
    return MatchDegree::Work;
}

void builtin_converter::FileSystem::conv(KernelObj *kobj) {
    auto *disk = dynamic_cast<LogicalDisk *>(kobj);
    if (!disk) return;

    auto info = disk->info();
    if (!info || info->type == 0) return;

    Cluster *fs;
    if (kmod->fs.exists(info->type)) {
        PartInfoPrint(info->type);
        fs = kmod->fs[info->type](disk);
    }
    else fs = new RAW{disk};
    driverletterspace->regist(fs);
}