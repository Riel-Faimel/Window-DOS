#include "bs.hpp"
#include <stdio>

[[noreturn]]
void BasicShell::main() {
    kout << " - renew DOS Basic Shell -\n";
    while(1) { asm volatile ("hlt"); }
}