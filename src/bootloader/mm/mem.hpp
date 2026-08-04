#ifndef __bootloader_mm_mem_hpp
#define __bootloader_mm_mem_hpp

#include <interface/dev_cls/cluster.hpp>

class mem : public Cluster {
public:
    unsigned read(void *, unsigned, unsigned, unsigned);
    unsigned write(void *, unsigned, unsigned , unsigned);
    Cluster_Info* info(String);
    unsigned cmd(unsigned, String, void *, unsigned);

    unsigned open(String);
    unsigned close(unsigned);
    unsigned create(String);
    unsigned del(String);

    void *mmap(String, void *);
};

#endif