#include "_mem.hpp"

/**
 * Create a named shared memory
 * Return memory base address
 */
unsigned Memory::create(String mapname) {
    return 0;
}

/**
 * Delete named sh-mem
 */
unsigned Memory::delet(String mapname) {
    return 0;
}

/**
 * Open a named shared memory
 * from name and return address
 */
unsigned Memory::open(String mapname) {
    return 0;
}

/**
 * Close a map
 * Return error code
 */
unsigned Memory::close(unsigned mapaddr) {
    return 0;
}

void *Memory::mmap(unsigned, void *) {
    return nullptr;
}