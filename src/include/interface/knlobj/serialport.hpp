#ifndef __include_interface_knlobj_serialport_hpp
#define __include_interface_knlobj_serialport_hpp

#include "knlobj.hpp"

class SerialPort : public KernelObj {
public:
    /**
     * take/give(buf, port, bytes)
     */
    virtual ~SerialPort() = default;
    virtual unsigned take(void *, unsigned, unsigned) = 0;
    virtual unsigned give(void *, unsigned, unsigned) = 0;
};


#endif