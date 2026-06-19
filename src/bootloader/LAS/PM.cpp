#include "_PM.hpp"

class SingalPart : public LogicalDisk {
    Cluster_Info info_ {Cluster_Info::Type::Part};
    size_t total_sectors;
    size_t start; //LBA
    LogicalDisk *disk;
public:
    SingalPart() = default;
    SingalPart(LogicalDisk *disk_, unsigned begin, unsigned size):
    disk{disk_}, info_{.total_bytes = size}{
        info_.cluster_size = disk->info({})->cluster_size;
        total_sectors = (info_.total_bytes + 1) / info_.cluster_size;
    };

    unsigned read(void *buf, unsigned LBA, unsigned, unsigned nums){
        unsigned read_num;
        if (nums > total_sectors) read_num = total_sectors;
        else read_num = nums; 
        return disk->read(buf, LBA + start, 0, read_num);
    }
    unsigned write(void *buf, unsigned LBA, unsigned, unsigned nums){
        unsigned write_num;
        if (nums > total_sectors) write_num = total_sectors;
        else write_num = nums; 
        return disk->write(buf, LBA + start, 0, write_num);
    }
    Cluster_Info* info(String){
        return &info_;
    }
    unsigned cmd(unsigned, String){
        return 0;
    }
};

class RAW : public Cluster {
    LogicalDisk* disk;
public:
    RAW(LogicalDisk *d): disk{d}{}
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
        auto re = disk->info(String{});
        re->type = Cluster_Info::Type::RAW;
        return re;
    }
    unsigned cmd(unsigned cmdid, String args){
        return disk->cmd(cmdid, args);
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
    if(mbr.sign == (unsigned short)0xAA55)
    for(unsigned char i = 0;i < 4;i++){
        auto sysid = mbr.part[i].system_id;
        if(sysid){
            kprint("System ID: ");print_hex(sysid);kprint("\n");
            size_t start = mbr.part[i].start_LBA_high << 16 | mbr.part[i].start_LBA_low;
            size_t size = mbr.part[i].sector_count_high << 16 | mbr.part[i].sector_count_low;
            auto part = new SingalPart{disk, start, size};
            disk_stack.append(part);

            // try file system
            Cluster *fs;
            switch (static_cast<System_ID>(sysid)) {
            case System_ID::FAT12:
                break;
            case System_ID::FAT16:
                fs = new FAT16{part};
                linear_address_space->regist(fs);
                break;
            default:
            case System_ID::Re:
            case System_ID::NULL:
                fs = new RAW{part};
                linear_address_space->regist(fs);
                break;
            }
        }
    }
    else { 
        kprint("No MBR:\n");
        unsigned char *ptr = reinterpret_cast<unsigned char *>(mbr.buf);
        for (unsigned i = 0;i < 512;i++) {
            print_hex(ptr[i], false);print_char(' ');
        }
        kprint("\n===\n");
        return;
    } // MBR no found
}