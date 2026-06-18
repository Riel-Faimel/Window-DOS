#include "_PM.hpp"

class SingalPart : public LogicalDisk {
    size_t start;
    size_t total_size;
    LogicalDisk *disk;
public:
    SingalPart() = default;
    SingalPart(LogicalDisk *disk_, unsigned begin, unsigned size):
    disk{disk_}, start{begin}, total_size{size}{};

    unsigned read(void *buf, unsigned, unsigned offset, unsigned nums){
        unsigned read_num;
        if (offset + nums > total_size) read_num = total_size - offset;
        else read_num = nums; 
        return disk->read(buf, 0, offset + start, read_num);
    }
    unsigned write(void *buf, unsigned, unsigned offset, unsigned nums){
        unsigned write_num;
        if (offset + nums > total_size) write_num = total_size - offset;
        else write_num = nums; 
        return disk->write(buf, 0, offset + start, write_num);
    }
    Cluster_Info* info(String s){
        return disk->info(s);
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

    unsigned read(void *buf, unsigned, unsigned from, unsigned nums) {
        return disk->read(buf, 0, from, nums);
    }

    unsigned write(void *buf, unsigned, unsigned from, unsigned nums) {
        return disk->write(buf, 0, from, nums);
    }
    unsigned open(String) { return 0; }
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

PM::PM():disk_stack{} {
    ;
}

PM::~PM() {
    ;
}

void PM::include(LogicalDisk *disk){
    disk_stack.append(disk);
}

void PM::resolve(LogicalDisk *disk) {
    MBR mbr;
    disk->read(mbr.buf, 0, 0, 512);
    if(mbr.sign == (unsigned short)0xAA55){
        for(unsigned char i = 0;i < 4;i++){
            auto sysid = mbr.part[i].system_id;
            if(sysid){
                size_t start = mbr.part[i].start_LBA_high << 16 | mbr.part[i].start_LBA_low;
                size_t size = mbr.part[i].sector_count_high << 16 | mbr.part[i].sector_count_low;
                auto part = new SingalPart{disk, start, size};
                disk_stack.append(part);

                switch (static_cast<System_ID>(sysid)) {
                case System_ID::FAT12:
                    break;
                case System_ID::FAT16:
                    //auto fat16 = new FAT16{};
                    break;
                case System_ID::NULL:
                default:
                    auto raw = new RAW{part};
                    ;
                    break;
                }
            }
        }
    }
    else { return; } // MBR no found
}