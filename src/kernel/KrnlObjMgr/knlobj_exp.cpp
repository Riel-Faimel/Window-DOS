#include <interface/knlobj/knlobj.hpp>

/**
 * 这几个析构函数是 key function：每个多态基类的 vtable 只在这里发源。
 * 本 TU 用 -frtti 编译（*_exp.cpp 规则），所以发出去的 vtable 带 typeinfo，
 * dynamic_cast 才能在这套 freestanding 运行时里工作。
 */
KernelObj::~KernelObj() = default;
PhysicalDisk::~PhysicalDisk() = default;
LogicalDisk::~LogicalDisk() = default;
SerialPort::~SerialPort() = default;
FramePort::~FramePort() = default;
