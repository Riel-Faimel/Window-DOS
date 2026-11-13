#include "geh.hpp"
#include <stdio>



namespace {

class GlobalExceptionHandler {
public:
    GlobalExceptionHandler() = default;
    void terminate(GlobalExceptionType);
};
GlobalExceptionHandler geh;

};