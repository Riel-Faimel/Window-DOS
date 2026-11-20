#include "cpu.hpp"
#include <id/id.hpp>

CPUList cpu_list;

arch::CPU::CPU() {
    interrupt_distributor.append_cpu(*this);
    cpu_list.append(this, cpuid);
}