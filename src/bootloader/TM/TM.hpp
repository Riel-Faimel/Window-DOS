#ifndef __bootloader_TM_TM_hpp
#define __bootloader_TM_TM_hpp

/**
 * TaskManager is global task start entry
 * task include thread and sources list
 */

#include <lib/cppstdlib/string>
#include <multiCE/Multi-Channel_exec.hpp>

class TaskManager {
public:
    TaskManager();

    unsigned exec(String, size_t, void *);
};

#endif