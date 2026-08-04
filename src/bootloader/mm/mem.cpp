#include "_mem.hpp"

/**
 * Create a named shared memory
 * Return memory base address
 */
unsigned mem::create(String mapname) {
    ;
}

/**
 * Delete named sh-mem
 */
unsigned mem::del(String mapname) {
    ;
}

/**
 * Open a named shared memory
 * from name and return address
 */
unsigned mem::open(String mapname) {
    ;
}

/**
 * Close a map
 * Return error code
 */
unsigned mem::close(unsigned mapaddr) {}

void *mmap(String, void *) {}