#include "_cpu.hpp"

CPU::CPU(volatile descrptor::IDTEntry *idtv, volatile descrptor::Entry *gdtv, int gdtc):
idt{idtv}, mm{}, gdt{gdtv, gdtc, idt} {}