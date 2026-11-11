#include <stdio>
#include <drv/screen/screen_srv.hpp>

iostream::iostream(): backend{&vgatextout} {}