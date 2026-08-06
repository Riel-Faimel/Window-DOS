#include "_TM.hpp"

extern "C" {
    __attribute__((naked))
    void prelude(size_t, void *){
        asm volatile (
            "call *%ebx\n"
            "int $0x21\n"
        );
    };
}

TaskManager::TaskManager() {}

void TaskManager::exec(String filepath) {
    WinHandle win{};
    if(driverletterspace->open(win, filepath) == (unsigned)-1) {
        //No such file
        return;
    }
    executor.run(&prelude, 1, nullptr);
}