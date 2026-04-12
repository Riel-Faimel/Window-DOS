#include "WIN.hpp"

_WIN::_WIN(){
    _WINDOW_MANAGER->regist(*this);
};
_WIN::~_WIN() noexcept{
    _WINDOW_MANAGER->destroy(*this);
}
