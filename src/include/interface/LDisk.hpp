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

struct DISK_INFO : public Cluster_Info {
    int total_sectors;
    int LBA_support;
    int PIO_supported;
    char model[41];
    u8 device; 
    u16 CHS_cylinders;
    u16 CHS_heads;
    u16 CHS_sectors_per_track;
    u32 multi_count;
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
    virtual Cluster_Info* info(String = {}) = 0;
    virtual unsigned cmd(unsigned, String, void *argv, unsigned argc) = 0;
};


#endif