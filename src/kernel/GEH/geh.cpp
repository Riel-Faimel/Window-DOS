#include "_geh.hpp"

void terminal(GlobalExceptionType type) {
    switch (type) {
    case GlobalExceptionType::UnwindBreak: {
        kout << "[GEH] Unwind Fault\n";
        while(1);
    } break;
    case GlobalExceptionType::BadMemoryAlloc: {
        ;
    } break;
    default: {
        ;
    } break;
    }
}