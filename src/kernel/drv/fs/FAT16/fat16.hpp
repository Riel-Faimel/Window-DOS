#ifndef __kernel_part_fs_FAT16_hpp
#define __kernel_part_fs_FAT16_hpp

#include <interface/dev_cls/cluster.hpp>

Cluster *create_fat_fs(LogicalDisk *);


#endif