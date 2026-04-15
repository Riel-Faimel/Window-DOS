#ifndef ____WIN
#define ____WIN

#include <global/WIN.hpp>

template<unsigned N = 0xFFFFFFFF>
class Window : _WIN{
private:
    unsigned char buffer[N];
public:
    Window(){
        size = N;
    };
    ~Window() = default;

    unsigned char operator[] (unsigned long long id)const{
        return buffer[id];
    }

    unsigned char at(unsigned long long id)const{
        if(id <= size)return buffer[id];
    }
};

template<>
class Window<0xFFFFFFFF> : _WIN{
private:
    unsigned char *buffer;
public:
    Window() = delete;
    template<unsigned N>
    Window(unsigned char (&buffer_init)[N]){
        buffer = buffer_init;
        size = N;
    }
    Window(Window<0xFFFFFFFF>&& win){
        handle = win.handle;
        flag = win.flag;
        extra = win.extra;
        size = win.size;
        buffer = win.buffer;

        win.handle = 0;
        win.flag = 0;
        win.extra = 0;
        win.size = 0;
        win.buffer = nullptr;
    }
    ~Window() = default;

    unsigned char operator[] (unsigned long long id)const{
        return buffer[id];
    }

    unsigned char at(unsigned long long id)const{
        if(buffer && id <= size)return buffer[id];
    }
};

#endif
