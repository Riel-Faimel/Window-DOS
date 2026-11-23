#pragma once

#include <global/type.hpp>

// Projection of a KernelObj for status/listing output. This is KOM-facing
// data, not part of the SVM instruction set.
enum class ObjKind : u8 {
    Phys,
    Part,
    Serial,
    Frame,
    Unknown,
};

struct ObjEntry {
    ObjKind kind;
    u32 a;
    u32 b;
    u32 c;
};
