#ifndef ____WIN
#define ____WIN

#include <global/WIN.hpp>

template<unsigned N>
class Window : _WIN {
private:
    unsigned char buffer[N];
public:
    Window(){
        size = N;
    }
    Window(_WIN *win):_WIN{win}{
        if(win != this){
            *win = {};
        }
    }
    Window(Window &) = delete;
    Window(Window &&win):Window{static_cast<_WIN *>(&win)}{}
    ~Window() = default;

    template<typename T>
    T operator[] (size_t id)const{
        return reinterpret_cast<T *>(buffer)[id];
    }

    template<typename T>
    T at(size_t id)const{
        if((id++) * sizeof(T) <= N)return reinterpret_cast<T *>(buffer)[id];
    }

    Window &operator= (_WIN *win){
        *static_cast<_WIN*>(this) = *win;
        *win = {};
        return *this;
    }
    Window &operator= (Window &&win){
        *static_cast<_WIN*>(this) = *static_cast<_WIN*>(&win);
        win = {};
        return *this;
    }

    Window &operator= (Window &) = delete;

    void *get_buffer(){
        return static_cast<void *>(buffer);
    }
};

/*
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
*/

#endif
