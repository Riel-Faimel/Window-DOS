//#include "../src/include/TL/idlib/idlib"
#include "exception.hpp"

//rtl::unsited_ptr<int> pointer;

int obj = 0;

int main(){
    int x = 3;
    //auto pointer = ::pointer(&x);
    try{
        throw "test";
    } catch (...) {
        return -1;
    }
    return 0;
}