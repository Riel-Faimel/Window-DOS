#include "_TM.hpp"

TaskManager::TaskManager() {}

void TaskManager::exec(String filepath) {
    _WIN win;
    driver_letter_space->open(win, filepath);
    ;
    executor.run(nullptr);
}