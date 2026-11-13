#ifndef __include_interface_knlobj_frameport_hpp
#define __include_interface_knlobj_frameport_hpp

#include "knlobj.hpp"

class FramePort : public KernelObj {
public:
    /**
     * send/recv(buf, port, bytes)
     */
    virtual ~FramePort() = default;
    virtual unsigned send(void *, unsigned, unsigned) = 0;
    virtual unsigned recv(void *, unsigned, unsigned) = 0;
};


#endif