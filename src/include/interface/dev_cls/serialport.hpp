#ifndef __include_interface_dev_cls_serialport_hpp
#define __include_interface_dev_cls_serialport_hpp

class SerialPort {
public:
    /**
     * take/give(buf, port, bytes)
     */
    virtual ~SerialPort() = default;
    virtual unsigned take(void *, unsigned, unsigned) = 0;
    virtual unsigned give(void *, unsigned, unsigned) = 0;
};


#endif