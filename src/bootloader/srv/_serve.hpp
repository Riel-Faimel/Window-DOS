#include "serve.hpp"
#include <drv/drv.hpp>

class Loader  : public _Loader{
    _Screen *screen;
public:
    Loader(Loader *, _Screen *);
    Loader() = default;
    void kprint(const char *message);
};