#ifndef __kernel_KOM_builtin_conv_part_hpp
#define __kernel_KOM_builtin_conv_part_hpp

#include <CDM/DLS.hpp>
#include <interface/srvdev/cluster.hpp>
#include <global/type.hpp>
#include <drv/kmod.hpp>


class SingalPart : public LogicalDisk {
    DiskInfo info_;
    size_t start; //LBA
    PhysicalDisk *disk;
public:
    SingalPart(PhysicalDisk *disk_, u64 begin, u64 size, u32 type):
    info_{ .su_start = begin, .su_num=size, .su_size=512, .type=type }, 
    start{begin}, disk{disk_}{};

    unsigned read(void *buf, unsigned LBA, unsigned, unsigned nums){
        unsigned read_num = 
        //*
        nums > info_.su_num ? info_.su_num - LBA : 
        //*/
        nums;
        /*
        kout << "reaing: ");print_hex(LBA+start);print_char(',');print_hex(read_num);print_char('\n';
        //*/
        return disk->read(buf, LBA + start, 0, read_num);
    }
    unsigned write(void *buf, unsigned LBA, unsigned, unsigned nums){
        unsigned write_num = 
        //*
        nums > info_.su_num ? info_.su_num - LBA : 
        //*/
        nums;
        return disk->write(buf, LBA + start, 0, write_num);
    }
    DiskInfo* info(unsigned, String){
        return &info_;
    }
    unsigned cmd(unsigned, String, void *, unsigned){
        return 0;
    }
};

class RAW : public Cluster {
    LogicalDisk* disk;
    DiskInfo info_ {.type=1};
public:
    RAW(LogicalDisk *d): disk{d}{
        auto i = disk->info();
        info_.su_size = i->su_size;
        info_.su_num = i->su_num;
    }
    ~RAW() = default;

    unsigned read(void *buf, unsigned LBA, unsigned, unsigned nums) {
        return disk->read(buf, LBA, 0, nums);
    }

    unsigned write(void *buf, unsigned LBA, unsigned, unsigned nums) {
        return disk->write(buf, LBA, 0, nums);
    }
    void *mmap(unsigned, void * = nullptr) override { return nullptr; };
    unsigned open(String s) { return (unsigned)-1; }
    unsigned close(unsigned) { return 0; }
    unsigned create(String) { return 0; }
    unsigned delet(String) { return 0; }
    DiskInfo* info(unsigned, String) {
        return &info_;
    }
    unsigned cmd(unsigned cmdid, String args, void *argv, unsigned argc){
        return disk->cmd(cmdid, args, argv, argc);
    }
};

#endif
