#include "_TM.hpp"

void caller(size_t n, void *v) {
    void *start_point = *(void **)v;
    void *argv = *((void **)v + 1);
    asm volatile (
        "mov %0, %%eax\n"
        "mov %1, %%edx\n"
        "call %2\n"
        "int $0x31\n" // process exit
        "int $0x32\n" // thread exit
        :
        : "r"(n), "r"(argv), "r"(start_point)
        : "eax", "edx", "memory"
    );
}

TaskManager::TaskManager() {}

unsigned TaskManager::exec(String filepath) {
    WinHandle win{};
    auto err = driverletterspace->open(win, filepath);
    if (err != 0) return (unsigned)-1;
    Multi_Channel_executor.run(caller, 0, nullptr);
    return 0;
}