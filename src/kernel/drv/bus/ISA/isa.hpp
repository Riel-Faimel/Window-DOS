#ifndef __kernel_PDS_ISA_isa_hpp
#define __kernel_PDS_ISA_isa_hpp

#include <interface/enumer.hpp>

class ISA_space : public Enumerator {
public:
    void probe() override;
};


#endif