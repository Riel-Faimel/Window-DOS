#include <arch/cpu.hpp>

class DOScall {
    IDT &idt;
public:
    DOScall(IDT&);
    ~DOScall() = default;
};