#include "pds.hpp"
#include <interface/hardev/device.hpp>
#include <interface/hardev/pift.hpp>
#define LocalDebug
#include <global/macro.hpp>
#include <stdio>

template<>
u32 hash<String> (const String &str) {
    u32 re = 0;
    for (auto ch : str) {
        re = re * 137 + ch;
    }
    return re;
}