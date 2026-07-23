#include "_TM.hpp"

TaskManager::TaskManager() {}

void TaskManager::exec(String filepath) {
    WinHandle win;
    if(driver_letter_space->open(win, filepath) == -1)
    screen->print("Not Found");
    else kprint("Found");
    executor.run(nullptr);
}