#include "serve.hpp"

__attribute__((section(".weak")))Loader *bootloader;

Loader::Loader(Loader *place, _Screen *screen_init){
    screen = screen_init;
}


void Loader::kprint(const char *message){
    ;
}
