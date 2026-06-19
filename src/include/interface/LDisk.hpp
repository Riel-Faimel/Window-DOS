#ifndef __include_interface_LDisk_hpp
#define __include_interface_LDisk_hpp

#include <global/type.hpp>
#include <cppstdlib/string>

struct Cluster_Info {
    enum class Type{
        IDE_Disk_driver,
        SATA_Disk_driver,

        Part,

        RAW,
        minFS,
        FAT16,
    } type;
    size_t total_bytes;
    size_t cluster_size;
};

/**
 * real disk and virtual disk also logical disk
 */
class LogicalDisk {
public:
    /**
     * if disk driver realize 
     * read/write(buffer, LBA, 0, sector_deal)
     * 
     * if File system realize
     * read/write(buffer, handle, byte_offset, byte_deal)
     */
    virtual unsigned read(void *, unsigned, unsigned, unsigned) = 0;
    virtual unsigned write(void *, unsigned, unsigned, unsigned) = 0;
    virtual Cluster_Info* info(String) = 0;
    virtual unsigned cmd(unsigned, String) = 0;
};


#endif