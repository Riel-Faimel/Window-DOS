#include "_TM.hpp"

TaskManager::TaskManager() {}

void TaskManager::exec(String filepath) {
    WinHandle win;
    driver_letter_space->open(win, filepath);
    auto start = driver_letter_space->mmap(win, filepath);
    executor.run(reinterpret_cast<void (*)(size_t, void *)>(start));
}