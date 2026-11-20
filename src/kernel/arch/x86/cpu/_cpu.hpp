#include "cpu.hpp"

#include <global/macro.hpp>
#include <global/new.hpp>
#include <HL/generator>
#include <HL/atomic>
#include <stdio>
#include <interface/mem/allocable.hpp>

namespace arch {
    
class inpage : public mem::Allocable {
public:
};

} // namespace arch 