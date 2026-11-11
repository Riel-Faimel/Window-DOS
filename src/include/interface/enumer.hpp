#ifndef __include_interface_enumer_hpp
#define __include_interface_enumer_hpp

struct BusIndex;
class Enumerator {
public:
    virtual ~Enumerator() = default;
    virtual void probe() = 0;
    // virtual DeviceType map(void *) = 0;
};

#endif