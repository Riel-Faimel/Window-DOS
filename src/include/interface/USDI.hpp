#include "Window.hpp"

class _USDI {
    virtual unsigned open(_WIN&, const char *) = 0;
    virtual unsigned close(_WIN&, const char *) = 0;
    virtual unsigned read(_WIN&, unsigned from, unsigned to) = 0;
    virtual unsigned write(_WIN&, unsigned from, unsigned to) = 0;
    virtual unsigned mkdir(_WIN&, const char *) = 0;
    virtual unsigned deldir(_WIN&, const char *) = 0;
    virtual unsigned creat(_WIN&, const char *) = 0;
    virtual unsigned del(_WIN&, const char *) = 0;
    virtual unsigned cmd(_WIN&, const char *) = 0;
};