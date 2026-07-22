#ifndef __bootloader_TM_TM_hpp
#define __bootloader_TM_TM_hpp

#include <lib/cppstdlib/string>
#include <multiCE/Multi-Channel_exec.hpp>

class TaskManager {
    multiCE executor;

public:
    TaskManager();

    void exec(String);
};

#endif