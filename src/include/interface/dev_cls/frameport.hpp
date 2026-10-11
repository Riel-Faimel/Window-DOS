#ifndef __include_interface_dev_cls_frameport_hpp
#define __include_interface_dev_cls_frameport_hpp

class FramePort {
public:
    /**
     * send/recv(buf, port, bytes)
     */
    virtual ~FramePort() = default;
    virtual unsigned send(void *, unsigned, unsigned) = 0;
    virtual unsigned recv(void *, unsigned, unsigned) = 0;
};


#endif