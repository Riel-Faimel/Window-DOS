#ifndef __include_interface_Window_hpp
#define __include_interface_Window_hpp

#include <global/WIN.hpp>

template<unsigned N>
class Window : WinHandle {
private:
    unsigned char buffer[N];
public:
    Window(): WinHandle{.size{N}} {}
    Window(Window &) = delete;
    Window(Window &&win): Window{static_cast<WinHandle &&>(win)}{}
    ~Window() = default;

    template<typename T>
    T operator[] (size_t id)const{
        return reinterpret_cast<T *>(buffer)[id];
    }
    template<typename T>
    T at(size_t id)const{
        if((id++) * sizeof(T) <= N)return reinterpret_cast<T *>(buffer)[id];
        else return T{};
    }

    Window &operator= (Window &) = delete;
    Window &operator= (Window &&win){
        *static_cast<WinHandle*>(this) = *static_cast<WinHandle*>(&win);
        win = {};
        return *this;
    }
};

template<>
class Window<0> : WinHandle{
private:
    unsigned char *buffer;
public:
    Window() = delete;
    template<unsigned N>
    Window(unsigned char (&buffer_init)[N]){
        buffer = buffer_init;
        size = N;
    }
    Window(Window<0>&& win){
        handle = win.handle;
        extra = win.extra;
        size = win.size;
        buffer = win.buffer;

        win.handle = 0;
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
