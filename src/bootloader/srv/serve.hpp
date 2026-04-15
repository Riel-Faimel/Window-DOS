#ifndef __bootloader_srv_serve_hpp
#define __bootloader_srv_serve_hpp
#include "serve_.hpp"
#include <interface/LOADER.hpp>
#include <drv/screen/interface.hpp>

class Loader  : public _Loader{
    _Screen *screen;
public:
    Loader(Loader *, _Screen *);
    Loader() = default;
    void kprint(const char *message);
};


#endif