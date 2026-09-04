#ifndef __bootloader_drv_fs_DFRT_dfrt_hpp
#define __bootloader_drv_fs_DFRT_dfrt_hpp

#include <interface/dev_cls/cluster.hpp>

Cluster *create_dfrt_fs(LogicalDisk *part);


#endif
