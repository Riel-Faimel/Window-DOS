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
    disk->read(mbr.buf, 0, 0, 1);
    /*
    for (unsigned i = 0;i < 512;i++) {
        print_hex(reinterpret_cast<unsigned char *>(mbr.buf)[i], false);print_char(' ');
    }
    //*/
    if(mbr.sign == (unsigned short)0xAA55)
    for(unsigned char i = 0;i < 4;i++){
        auto sysid = mbr.part[i].system_id;
        if(sysid){
            size_t start = mbr.part[i].start_LBA_high << 16 | mbr.part[i].start_LBA_low;
            size_t size = mbr.part[i].sector_count_high << 16 | mbr.part[i].sector_count_low;
            auto part = new SingalPart{disk, start, size};
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
        disk_stack.append(disk);
    } // MBR no found
}