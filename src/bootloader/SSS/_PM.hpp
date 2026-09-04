#include "PM.hpp"

#include <SSS/DLS.hpp>
#include <interface/dev_cls/cluster.hpp>
#include <global/type.hpp>
#include <drv/kmod.hpp>

#include <drv/screen/screen_srv.hpp>


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
    void *mmap(String, void * = nullptr){};
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
