#ifndef __kernel_KOM_kom_hpp
#define __kernel_KOM_kom_hpp

#include <interface/knlobj/physicaldisk.hpp>
#include <interface/knlobj/logicaldisk.hpp>
#include <interface/knlobj/serialport.hpp>
#include <interface/knlobj/frameport.hpp>

class KernelObjectManager {
public:
    void append(PhysicalDisk *);
    void append(LogicalDisk *);
    void append(SerialPort *);
    void append(FramePort *);
};


#endif