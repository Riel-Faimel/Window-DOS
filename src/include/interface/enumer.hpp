#ifndef __include_interface_enumer_hpp
#define __include_interface_enumer_hpp

class Enumerator {
public:
    virtual void probe() = 0;
    virtual void set_driver() = 0;
    //virtual void set_config(void *config) = 0;
};

#endif