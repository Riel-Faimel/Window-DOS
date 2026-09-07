#ifndef __bootloader_mm_mem_hpp
#define __bootloader_mm_mem_hpp

#include <interface/dev_cls/cluster.hpp>

class mem : public Cluster {
public:
    unsigned read(void *, unsigned, unsigned, unsigned) override;
    unsigned write(void *, unsigned, unsigned , unsigned) override;
    ClusterInfo* info(unsigned, String) override;
    unsigned cmd(unsigned, String, void *, unsigned) override;

    unsigned open(String) override;
    unsigned close(unsigned) override;
    unsigned create(String) override;
    unsigned delet(String) override;

    void *mmap(unsigned, void *) override;
};

#endif