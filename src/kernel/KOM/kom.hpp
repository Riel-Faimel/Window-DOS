#ifndef __kernel_KOM_kom_hpp
#define __kernel_KOM_kom_hpp

#include <interface/dev_cls/physicaldisk.hpp>
#include <interface/dev_cls/logicaldisk.hpp>
#include <interface/dev_cls/serialport.hpp>
#include <interface/dev_cls/frameport.hpp>

class KernelObjectManager {
public:
    void append(PhysicalDisk *);
    void append(LogicalDisk *);
    void append(SerialPort *);
    void append(FramePort *);
};


#endif