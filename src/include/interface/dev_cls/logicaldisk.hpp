#ifndef __include_interface_logicaldisk_hpp
#define __include_interface_logicaldisk_hpp

#include <global/type.hpp>
#include <cppstdlib/string>

struct ClusterInfo {
    size_t index_start;
    size_t index_num;
    size_t index_size;
    size_t type = 0;
};

struct DiskInfo : ClusterInfo {
    bool LBA_support;
    bool PIO_supported;
    char model[41];
    u8 device; 
    u16 CHS_cylinders;
    u16 CHS_heads;
    u16 CHS_sectors_per_track;
    u32 multi_count;
};

struct FileInfo : ClusterInfo {
    String name;
    u16 create_date;
    u16 create_time;
    u16 write_date;
    u16 write_time;
    u16 access_date;

    u32 xattr1;
    u32 xattr2;
};

/**
 * real disk and virtual disk also logical disk
 */
class LogicalDisk {
public:
    /**
     * if disk driver realize 
     * read/write(buffer, LBA, byte_offset, sector_deal)
     * 
     * if File system realize
     * read/write(buffer, handle, byte_offset, byte_deal)
     */
    virtual unsigned read(void *, unsigned, unsigned, unsigned) = 0;
    virtual unsigned write(void *, unsigned, unsigned, unsigned) = 0;
    virtual ClusterInfo* info(unsigned = 0, String = {}) = 0;
    virtual unsigned cmd(unsigned, String, void *argv, unsigned argc) = 0;
};


#endif