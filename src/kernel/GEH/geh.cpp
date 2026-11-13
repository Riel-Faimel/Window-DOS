#include "_geh.hpp"

void GlobalExceptionHandler::terminate(GlobalExceptionType type) {
    switch (type) {
    case GlobalExceptionType::UnwindBreak: {
        kout << "[GEH] <Unwind Fault>\n";
        while(1) { asm volatile ("hlt"); }
    } break;
    case GlobalExceptionType::BadMemoryAlloc: {
        ;
    } break;
    default: {
        ;
    } break;
    }
}

void terminate(GlobalExceptionType type) {
    geh.terminate(type);
}