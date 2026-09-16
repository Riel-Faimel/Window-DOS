#include "PM.hpp"

#include <SSS/DLS.hpp>
#include <interface/dev_cls/cluster.hpp>
#include <global/type.hpp>
#include <drv/kmod.hpp>

#define LocalDebug
#include <global/macro.hpp>
#include <drv/screen/screen_srv.hpp>


class SingalPart : public LogicalDisk {
    ClusterInfo info_;
    size_t start; //LBA
    LogicalDisk *disk;
public:
    SingalPart(LogicalDisk *disk_, u64 begin, u64 size):
    info_{ .index_start = begin, .index_num=size, .index_size=0, .type=0, }, 
    start{begin}, disk{disk_}{
        info_.index_size = disk->info()->index_size;
    };

    unsigned read(void *buf, unsigned LBA, unsigned, unsigned nums){
        unsigned read_num = 
        //*
        nums > info_.index_num ? info_.index_num - LBA : 
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
        nums > info_.index_num ? info_.index_num - LBA : 
        //*/
        nums;
        return disk->write(buf, LBA + start, 0, write_num);
    }
    ClusterInfo* info(unsigned, String){
        return &info_;
    }
    unsigned cmd(unsigned, String, void *, unsigned){
        return 0;
    }
};

class RAW : public Cluster {
    LogicalDisk* disk;
    ClusterInfo info_ {.type=1};
public:
    RAW(LogicalDisk *d): disk{d}{
        auto i = disk->info();
        info_.index_size = i->index_size;
        info_.index_num = i->index_num;
    }
    ~RAW() = default;

    unsigned read(void *buf, unsigned LBA, unsigned, unsigned nums) {
        return disk->read(buf, LBA, 0, nums);
    }

    unsigned write(void *buf, unsigned LBA, unsigned, unsigned nums) {
        return disk->write(buf, LBA, 0, nums);
    }
    void *mmap(unsigned, void * = nullptr) override { return nullptr; };
    unsigned open(String s) { return static_cast<unsigned>(s.to_int()); }
    unsigned close(unsigned) { return 0; }
    unsigned create(String) { return 0; }
    unsigned delet(String) { return 0; }
    ClusterInfo* info(unsigned, String) {
        return &info_;
    }
    unsigned cmd(unsigned cmdid, String args, void *argv, unsigned argc){
        return disk->cmd(cmdid, args, argv, argc);
    }
};
