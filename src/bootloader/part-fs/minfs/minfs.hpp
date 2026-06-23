#ifndef __bootloader_part_fs_minfs_minfs_hpp
#define __bootloader_part_fs_minfs_minfs_hpp

#include <interface/cluster.hpp>

class MinFS : Cluster {
    struct BPB {
        ;
    };
public:
    unsigned open(String);
    unsigned close(unsigned);
};

#endif