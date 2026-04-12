#define _BITS_32
#include <global/type.hpp>

class _ptr2size;

class mm{
private:
    unsigned block_size;
    unsigned total_blocks;
    void *base_address;
    bool *bitmap;
    _ptr2size *alloclist;
public:
    mm() = default;
    mm(
        unsigned, unsigned, void *, bool *,
        unsigned size = 1024
    );
    void *alloc(size_t);
    void dealloc(void *);
};

extern mm *memorymanager_objectpointer;

void *operator new(size_t);
void *operator new(size_t, void *);
void operator delete(void *);
void operator delete[](void *);
void operator delete[](void *, unsigned);