#include "_isa.hpp"

InsideModuleRecord EnumerMod head {
    []() -> Enumerator * { return new ISA_space{}; }
};

void ISA_space::probe() {
    kout << "[ISA] ready\n";
}