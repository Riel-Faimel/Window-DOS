#ifndef __include_interface_physicaldisk_hpp
#define __include_interface_physicaldisk_hpp

#include <global/type.hpp>
#include <string>

/**
 * real disk and virtual disk also logical disk
 */
class PhysicalDisk {
public:
    /**
     * if disk driver realize 
     * read/write(buffer, LBA, byte_offset, sector_deal)
     */
    virtual ~PhysicalDisk() = default;
    virtual unsigned read(void *, unsigned, unsigned, unsigned) = 0;
    virtual unsigned write(void *, unsigned, unsigned, unsigned) = 0;
};


#endif