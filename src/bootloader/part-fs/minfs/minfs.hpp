#ifndef __bootloader_part_fs_minfs_minfs_hpp
#define __bootloader_part_fs_minfs_minfs_hpp
#include <part-fs/fs.hpp>

class MinFS : FileSystem {
    struct BPB {
        ;
    };
public:
    unsigned open(String);
    void close(unsigned);

    void cd(String);
    void dir();
};

#endif