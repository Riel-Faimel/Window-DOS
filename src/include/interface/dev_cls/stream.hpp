#ifndef __include_interface_kernel_stream_hpp
#define __include_interface_kernel_stream_hpp
#include <global/type.hpp>

class Stream {
public:
    virtual ~Stream() = default;
    virtual Stream &operator<<(u8) = 0;
    virtual Stream &operator>>(u8&) = 0;

    virtual void send() = 0;
    virtual void get() = 0;
    virtual void control() = 0;
    virtual void status() = 0;
};

#endif