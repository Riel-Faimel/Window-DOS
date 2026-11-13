#include "kmod.hpp"
#include <drv/bus/PCI/pci.hpp>
#include <drv/bus/ISA/isa.hpp>
#include <interface/hardev/drvhead.hpp>
#include <PDS/pds.hpp>
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
            case ModHead::ModType::PDevDrv: {
                auto realptr = (PDevDrv *)(ptr);
                ++realptr;
                ptr = static_cast<ModHead *>(realptr);
            } break;
            case ModHead::ModType::KrnlObj: {
                auto realptr = (KrnlObj *)(ptr);
                ++realptr;
                ptr = static_cast<ModHead *>(realptr);
            } break;
            case ModHead::ModType::SrvDev: {
                auto realptr = (SrvDev *)(ptr);
                ++realptr;
                ptr = static_cast<ModHead *>(realptr);
            } break;
            default:{
                throw 1;
            } break;
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