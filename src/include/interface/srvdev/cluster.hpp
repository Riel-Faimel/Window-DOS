#ifndef __include_interface_srvdev_cluster_hpp
#define __include_interface_srvdev_cluster_hpp

#include <string>

class Cluster {
public:
    struct ClusterInfo {};

    virtual ~Cluster() = default;
    virtual unsigned read(void *, unsigned, unsigned, unsigned) = 0;
    virtual unsigned write(void *, unsigned, unsigned, unsigned) = 0;
    virtual unsigned open(String) = 0;
    virtual unsigned close(unsigned) = 0;
    virtual unsigned create(String) = 0;
    virtual unsigned delet(String) = 0;
    virtual void *mmap(unsigned, void *) = 0;
    virtual ClusterInfo* info(unsigned = 0, String = {}) = 0;
    virtual unsigned cmd(unsigned, String, void *argv, unsigned argc) = 0;
};

#endif