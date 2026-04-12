#include <iostream>
#include "src/interpret/UIDI.hpp"
#include "windows.h"
#include <thread>

struct promise_test : Promise<promise_test, int>{
    std::suspend_never initial_suspend(){
        return {};
    }
    std::suspend_never final_suspend()noexcept{
        return {};
    }
    void return_value(int r){
        std::cout << "exiting..\n";
    }
};

struct sth_ : _awaitable<sth_> {
    bool await_ready(){return false;}
    void await_suspend(std::coroutine_handle<> h){
        //std::cout << "挂起准备\n";
    }
    void await_resume(){
        //std::cout << "挂起恢复\n";
    }
};

using Task = _task<promise_test>;




Task run(int &i){
    while(1){
        co_await sth_{};
        i = (i+1) % 4;
        //std::cout << ">>>" << i << "\n";
    }
    co_return 0;
}
void I(int &i){
    while(1){
        Sleep(10);
        switch (i){
        case 0:
            std::cout << "-\n";
            break;
        case 1:
            std::cout << "/\n";
            break;
        case 2:
            std::cout << "|\n";
            break;
        case 3:
            std::cout << "\\\n";
            break;
        default:
            break;
        }
    }
}

int main(){
    int i = 0;

    auto t = run(i);

    std::thread th_([t]{
        while(1){
            Sleep(50);
            const_cast<Task&>(t).resume();
        }
    });

    I(i);

    th_.join();
    std::cout << "end\n";
    return 0;
}