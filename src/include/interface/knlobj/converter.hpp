#ifndef __include_interface_newobj_hpp
#define __include_interface_newobj_hpp

#include "knlobj.hpp"

class Converter {
public:
    enum MatchDegree {
        MatchWell,  // catch object
        MatchBad,   // catch object but failed
        Match,      // can use
        Work,       // can use(low)
        Unknow,     // can't use
    };
    virtual MatchDegree touch(KernelObj *) = 0;
    virtual void conv(KernelObj *) = 0;
    virtual ~Converter() = default;
};



#endif