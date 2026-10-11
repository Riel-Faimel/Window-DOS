#ifndef __kernel_TM_TM_hpp
#define __kernel_TM_TM_hpp

/**
 * TaskManager is global task start entry
 * task include thread and sources list
 */

#include <lib/string>
#include <multiCE/Multi-Channel_exec.hpp>

enum class FexecExceptionCode : u8 {
    // done
    NoException,

    // exception
    OpenFileFailed,

    // fault
    DLSFault,
};

class TaskManager {
public:
    TaskManager();

    FexecExceptionCode exec(String, size_t, void *);
};

#endif