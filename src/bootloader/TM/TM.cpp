#include "_TM.hpp"

extern "C" {
    __attribute__((naked))
    void hello(size_t, void *){
        cout << "hello\n";
        _Ssche();
        while(1) { asm volatile ("hlt\n"); }
        asm volatile (
            "int $48\n"
        );
    };
    __attribute__((naked))
    void next(size_t, void *){
        cout << "next\n";
        _Ssche();
    };
    __attribute__((naked))
    void ok(size_t, void *){
        cout << "ok\n";
        _Ssche();
    };
}

TaskManager::TaskManager() {}

void TaskManager::exec(String filepath) {
    WinHandle win{};
    if(driverletterspace->open(win, filepath) == (unsigned)-1) {
        //No such file
        return;
    }
    Multi_Channel_executor.run((void *)&hello, 0, nullptr, 0);
    Multi_Channel_executor.run((void *)&next, 0, nullptr, 0);
    Multi_Channel_executor.run((void *)&ok, 0, nullptr, 0);
}