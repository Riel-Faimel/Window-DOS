#ifndef __include_interface_bootoader_cluster_hpp
#define __include_interface_bootoader_cluster_hpp

#include <interface/LDisk.hpp>
#include <cppstdlib/string>

class Cluster : public LogicalDisk {
public:
    /**
     * if disk driver realize 
     * read/write(buffer, LBA, byte_offset, byte_deal)
     * 
     * if File system realize
     * read/write(buffer, handle, byte_offset, byte_deal)
     */
    virtual unsigned open(String) = 0;
    virtual unsigned close(unsigned) = 0;
    virtual unsigned create(String) = 0;
    virtual unsigned delet(String) = 0;
};

#endif