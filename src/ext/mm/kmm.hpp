#define _BITS_64
#include <global/type.hpp>

class KMM {
    void *alloc(size_t);
    void free(void *);
};