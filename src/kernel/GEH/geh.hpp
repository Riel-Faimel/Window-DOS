#ifndef __kernel_GEH_geh_hpp
#define __kernel_GEH_geh_hpp

#include <global/type.hpp>

enum class GlobalExceptionType : u32 {
BadMemoryAlloc,
InvalidTCB,
};

extern "C" void terminal(GlobalExceptionType);


#endif