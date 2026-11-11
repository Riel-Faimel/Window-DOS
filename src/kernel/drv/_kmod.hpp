#include "kmod.hpp"
#include "drv.hpp"
#include "mod_head.hpp"
#include <PDS/pds.hpp>
#include <global/type.hpp>

class ModHeadPtr {
    u8 *ptr;
public:
    ModHeadPtr(): ptr{nullptr} {}
    ModHeadPtr(void *ptr_): ptr{static_cast<u8 *>(ptr_)} {}
    
    ModHeadPtr &operator = (void *ptr_) { ptr = static_cast<u8 *>(ptr_); return *this; }
    ModHeadPtr &operator += (const ModHead &) { ptr += sizeof(ModHead); return *this; }
    ModHeadPtr &operator += (const PDevDrv &) { ptr += sizeof(PDevDrv); return *this; }

    bool operator != (const ModHeadPtr &oth) const { return ptr != oth.ptr; }
};

extern "C" ModHeadPtr __inside_mod_begin;
extern "C" ModHeadPtr __inside_mod_end;