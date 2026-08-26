#include "_TM.hpp"

extern "C" {
    void test_call(size_t, void *) {
        while(1)
        asm volatile (
            "mov %0, %%dl\n"
            "mov $0x02, %%ah\n"
            "int $0x21\n"
            :
            : "N"('A')
            : "dl", "ah", "memory"
        );
    }
    void test_call_(size_t, void *) {
        while(1)
        asm volatile (
            "mov %0, %%dl\n"
            "mov $0x02, %%ah\n"
            "int $0x21\n"
            :
            : "N"('B')
            : "dl", "ah", "memory"
        );
    }
    void hello(size_t, void *) {
        while(1)
        cout << "^^^^^";
    }
    void next(size_t, void *){
        while(1)
        cout << "|||||";
    };
    void ok(size_t, void *){
        while(1)
        cout << "_____";
    };
}

TaskManager::TaskManager() {}

void TaskManager::exec(String filepath) {
    WinHandle win{};
    if(driverletterspace->open(win, filepath) == (unsigned)-1) {
        //No such file
        return;
    }
    //Multi_Channel_executor.run(&test_call, 0, nullptr, 1, 3);
    //Multi_Channel_executor.run(&test_call_, 0, nullptr, 2, 0);
}