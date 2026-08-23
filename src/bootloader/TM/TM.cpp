#include "_TM.hpp"

extern "C" {
    void _Shello();
    void _Snext();
    void _Sok();
    void hello() {
        cout << "hello\n";
    }
    void next(size_t, void *){
        cout << "next\n";
    };
    void _Cok(size_t, void *){
        cout << "ok\n";
    };
}

TaskManager::TaskManager() {}

void TaskManager::exec(String filepath) {
    WinHandle win{};
    if(driverletterspace->open(win, filepath) == (unsigned)-1) {
        //No such file
        return;
    }
    Multi_Channel_executor.run((void *)&_Shello, 0, nullptr, 0, 10);
    Multi_Channel_executor.run((void *)&_Snext, 0, nullptr, 3);
    Multi_Channel_executor.run((void *)&_Sok, 0, nullptr, 0);
}