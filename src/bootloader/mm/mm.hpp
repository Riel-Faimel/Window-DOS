#ifndef __bootloader_mm_mm_hpp
#define __bootloader_mm_mm_hpp

#include <global/type.hpp>
#include <global/new.hpp>

class _ptr2size;

class MemoryManager{
private:
    unsigned block_size;
    unsigned total_blocks;
    void *base_address;
    bool *bitmap;
    _ptr2size *alloclist;
public:
    MemoryManager(){};
    MemoryManager(
        unsigned, unsigned, void *, bool *,
        unsigned size = 1024
    );
    void *alloc(size_t);
    void dealloc(void *);
};

extern MemoryManager *memorymanager_objectpointer;

#endif