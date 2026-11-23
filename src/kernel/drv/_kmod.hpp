#include "kmod.hpp"
#include <PDS/pds.hpp>
#include <KrnlObjMgr/kom.hpp>
#include <global/type.hpp>

extern "C" {
    extern ModHead __inside_mods_begin;
    extern ModHead __inside_mods_end;
}

namespace {

class {
public:
    class Iterator {
        ModHead *ptr = &__inside_mods_begin;
    public:
        ModHead &operator *() const { return *ptr; }
        Iterator &operator ++ () {
            switch (ptr->mod_type) {

            #define ModuleTypeBanch(...)\
            case ModHead::ModType::__VA_ARGS__: {\
                auto realptr = (__VA_ARGS__ *)(ptr);\
                ++realptr;\
                ptr = static_cast<ModHead *>(realptr);\
            } break;

            ModuleTypeBanch(PDevDrv)
            ModuleTypeBanch(KrnlObj)
            ModuleTypeBanch(EnumerMod)
            ModuleTypeBanch(SyscallMod)
            default:{
                throw 1;
            } break;
            
            #undef ModuleTypeBanch
            }
            return *this;
        }
        bool operator != (const Iterator &) const {
            if (ptr == &__inside_mods_end) return false;
            else if (ptr > &__inside_mods_end) throw 2;
            else return true;
        }
    };

    auto begin() const { return Iterator{}; }
    auto end() const { return Iterator{}; }
} inside_mod_tranv;

};