#include "_PM.hpp"

PM *partmanager;

PM::PM():disk_stack{} {
    partmanager = this;
}

void PM::include(LogicalDisk *disk){
    disk_stack.append(disk);
}

void PM::resolve(LogicalDisk *disk) {
    MBR mbr;
    u8 try_times = 0;
resolv_MBR:
    disk->read(mbr.buf, 0, 0, 1);
    if(mbr.sign == (unsigned short)0xAA55)
    for(unsigned char i = 0;i < 4;i++){
        auto sysid = mbr.part[i].system_id;
        if(sysid){
            size_t start = mbr.part[i].start_LBA_high << 16 | mbr.part[i].start_LBA_low;
            size_t size = mbr.part[i].sector_count_high << 16 | mbr.part[i].sector_count_low;
            auto part = new SingalPart{disk, (u64)start, (u64)size};
            disk_stack.append(part);

            // try file system
            cout << "[INFO] Part found, ID: " << sysid << ", [" << start << "]\n";
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
    else {
        if (try_times > 4) return;
        else {
            try_times++;
            goto resolv_MBR;
        }
        LocalDebugInfo(
            for (auto ch : mbr.buf) {
                print_hex(ch, false);print_char(' ');
            }
        );
        disk_stack.append(disk);
    } // MBR no found
}