#pragma once

#include <global/type.hpp>

// Projection of a DLS drive for status/listing output. This is srvdev-facing
// data, not part of the SVM instruction set.
struct DriveEntry {
    u32 letter;
    u32 cid;
};
