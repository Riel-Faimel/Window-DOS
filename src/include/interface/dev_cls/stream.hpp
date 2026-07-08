#ifndef __include_interface_bootloader_stream_hpp
#define __include_interface_bootloader_stream_hpp
#include <global/type.hpp>

class Stream {
    virtual Stream &operator<<(u8) = 0;
    virtual Stream &operator>>(u8&) = 0;

    virtual void send();
    virtual void get();
    virtual void control();
    virtual void status();
};

#endif