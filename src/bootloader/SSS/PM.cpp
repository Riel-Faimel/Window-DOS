#include "_PM.hpp"

class SingalPart : public LogicalDisk {
    Cluster_Info info_;
    size_t total_sectors;
    size_t start; //LBA
    LogicalDisk *disk;
public:
    SingalPart(LogicalDisk *disk_, unsigned begin, unsigned size):
    info_{ .type=Cluster_Info::Type::Part, .total_bytes=size, .cluster_size=0 }, 
    start{begin}, disk{disk_}{
        info_.cluster_size = disk->info()->cluster_size;
        total_sectors = (info_.total_bytes + 1) / info_.cluster_size;
    };

    unsigned read(void *buf, unsigned LBA, unsigned, unsigned nums){
        unsigned read_num = 
        //*
        nums > total_sectors ? total_sectors - LBA : 
        //*/
        nums;
        /*
        kprint("reaing: ");print_hex(LBA+start);print_char(',');print_hex(read_num);print_char('\n');
        //*/
        return disk->read(buf, LBA + start, 0, read_num);
    }
    unsigned write(void *buf, unsigned LBA, unsigned, unsigned nums){
        unsigned write_num = 
        //*
        nums > total_sectors ? total_sectors - LBA : 
        //*/
        nums;
        return disk->write(buf, LBA + start, 0, write_num);
    }
    Cluster_Info* info(String){
        return &info_;
    }
    unsigned cmd(unsigned, String, void *, unsigned){
        return 0;
    }
};

class RAW : public Cluster {
    LogicalDisk* disk;
    Cluster_Info info_ {.type=Cluster_Info::Type::RAW};
public:
    RAW(LogicalDisk *d): disk{d}{
        auto i = disk->info();
        info_.cluster_size = i->cluster_size;
        info_.total_bytes = i->total_bytes;
    }
    ~RAW() = default;

    unsigned read(void *buf, unsigned LBA, unsigned, unsigned nums) {
        return disk->read(buf, LBA, 0, nums);
    }

    unsigned write(void *buf, unsigned LBA, unsigned, unsigned nums) {
        return disk->write(buf, LBA, 0, nums);
    }
    unsigned open(String s) { return static_cast<unsigned>(s.to_int()); }
    unsigned close(unsigned) { return 0; }
    unsigned create(String) { return 0; }
    unsigned delet(String) { return 0; }
    Cluster_Info* info(String) {
        return &info_;
    }
    unsigned cmd(unsigned cmdid, String args, void *argv, unsigned argc){
        return disk->cmd(cmdid, args, argv, argc);
    }
};

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
        kprint("System ID: ");print_hex(sysid);kprint("\n");
            Cluster *fs;
            switch (static_cast<System_ID>(sysid)) {
            case System_ID::FAT12:
                break;
            case System_ID::FAT32:
            case System_ID::FAT32_:
                break;
            case System_ID::FAT16:
            case System_ID::FAT16_:
            case System_ID::FAT16__:
                fs = new FAT16{part};
                driver_letter_space->regist(fs);
                break;
            default:
            case System_ID::Re:
            case System_ID::NULL:
                fs = new RAW{part};
                driver_letter_space->regist(fs);
                break;
            }
        }
    }
    else { 
        disk_stack.append(disk);
    } // MBR no found
}