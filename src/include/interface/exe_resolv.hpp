#ifndef __include_interface_exec_form_hpp
#define __include_interface_exec_form_hpp

#include <interface/Window.hpp>

class ExecLoader {
public:
    virtual bool execable(const WinHandle &) = 0;
    virtual void load() = 0;
    virtual void *get_entry_point() const = 0;
    virtual ~ExecLoader() = default;
};


#endif