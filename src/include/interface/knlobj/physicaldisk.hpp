#ifndef __include_interface_knlobj_physicaldisk_hpp
#define __include_interface_knlobj_physicaldisk_hpp

#include <global/type.hpp>
#include <string>
#include "knlobj.hpp"

/**
 * real disk and virtual disk also logical disk
 */
class PhysicalDisk : public KernelObj {
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