#ifndef __include_interface_bootoader_cluster_hpp
#define __include_interface_bootoader_cluster_hpp

#include <interface/LDisk.hpp>
#include <cppstdlib/string>

class Cluster : public LogicalDisk {
public:
    virtual unsigned open(String) = 0;
    virtual unsigned close(unsigned) = 0;
    virtual unsigned create(String) = 0;
    virtual unsigned delet(String) = 0;
};

#endif