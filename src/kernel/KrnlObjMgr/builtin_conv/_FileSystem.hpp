#include "FileSystem.hpp"
#include <interface/srvdev/cluster.hpp>
#include <SrvDevMgr/DLS.hpp>
#include <drv/kmod.hpp>

class RAW : public Cluster {
    LogicalDisk* disk;
    ClusterInfo info_ {};
public:
    RAW(LogicalDisk *d): disk{d}{
        // auto i = disk->info();
        // info_.su_size = i->su_size;
        // info_.su_num = i->su_num;
    }
    ~RAW() = default;

    unsigned read(void *buf, unsigned LBA, unsigned, unsigned nums) {
        return disk->read(buf, LBA, 0, nums);
    }

    unsigned write(void *buf, unsigned LBA, unsigned, unsigned nums) {
        return disk->write(buf, LBA, 0, nums);
    }
    void *mmap(unsigned, void * = nullptr) override { return nullptr; };
    unsigned open(String) { return (unsigned)-1; }
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
