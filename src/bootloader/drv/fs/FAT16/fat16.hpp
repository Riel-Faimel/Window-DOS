#ifndef __bootloader_part_fs_FAT16_hpp
#define __bootloader_part_fs_FAT16_hpp

#include <interface/dev_cls/cluster.hpp>

Cluster *create_fat_fs(LogicalDisk *);


#endif