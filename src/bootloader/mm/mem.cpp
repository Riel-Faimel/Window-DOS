#include "_mem.hpp"
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * Create a named shared memory
 * Return memory base address
 */
unsigned mem::create(String mapname) {
    return 0;
}

/**
 * Delete named sh-mem
 */
unsigned mem::delet(String mapname) {
    return 0;
}

/**
 * Open a named shared memory
 * from name and return address
 */
unsigned mem::open(String mapname) {
    return 0;
}

/**
 * Close a map
 * Return error code
 */
unsigned mem::close(unsigned mapaddr) {
    return 0;
}

void *mmap(String, void *) {
    return nullptr;
}