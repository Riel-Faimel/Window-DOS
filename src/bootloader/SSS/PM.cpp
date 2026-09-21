#include "_PM.hpp"

PM *partmanager;

static void PartInfoPrint(u8 sysid) {
    cout << "[INFO] ";
    switch (sysid) {
    case 0x0E:
        cout << "FAT16";
        break;
    default:
        cout << "RAW";
        break;
    }
    cout << " Part Got, ";
}

PM::PM():disk_stack{} {
    partmanager = this;
}

void PM::include(LogicalDisk *disk){
    disk_stack.append(disk);
}

void PM::resolve(LogicalDisk *disk) {
    MBR mbr;
    static_assert(sizeof(MBR) == 512);
    for (auto timeout : rtl::range{4}) {
        auto err = disk->read(mbr.buf, 0, 0, 1);
        if (err == -1) {
            LocalDebugInfo(
                cout << "Read Failed\n";
                break;
            )
        }
        if(mbr.sign == (unsigned short)0xAA55) {
            for(unsigned char i = 0;i < 4;i++){
                auto sysid = mbr.part[i].system_id;
                if(sysid){
                    size_t start = mbr.part[i].start_LBA_high << 16 | mbr.part[i].start_LBA_low;
                    size_t size = mbr.part[i].sector_count_high << 16 | mbr.part[i].sector_count_low;
                    auto part = new SingalPart{disk, (u64)start, (u64)size};
                    disk_stack.append(part);

                    // try file system
                    DebugInfo(PartInfoPrint(sysid);)
                    Cluster *fs;
                    if (kmod->fs.exists(sysid)) {
                        fs = kmod->fs[sysid](part);
                        driverletterspace->regist(fs);
                    } else {
                        fs = new RAW{part};
                        driverletterspace->regist(fs);
                    }
                }
            }
            return;
        }
    }
    LocalDebugInfo(
        for (auto ch : mbr.buf) {
            print_hex(ch, false);print_char(' ');
        }
    );
    disk_stack.append(disk);
    // MBR no found
}