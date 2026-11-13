#include "_kmod.hpp"

KernelModule *kmod = nullptr;

template<>
size_t hash(const size_t &i) { return i; }

KernelModule::KernelModule():
fs{} {
    kmod = this;
    pdevspace->append(this);
    reg_insmod();
}

void KernelModule::reg_insmod() {
    for (auto &head : inside_mod_tranv) {
        switch (head.mod_type) {
        case ModHead::ModType::PDevDrv: {
            auto drv = (PDevDrv &)head;
            switch (drv.drv_t) {
            case PDevDrv::DrvType::ProgIF: {
                pdevspace->append(drv.pid, drv.drvc);
            } break;
            case PDevDrv::DrvType::Dev: {
                pdevspace->append(drv.pid, drv.dev_t, drv.drvc);
            } break;
            default: {
                while(1);
            } break;
            }
        } break;
        case ModHead::ModType::KrnlObj: {
            ;
        } break;
        case ModHead::ModType::SrvDev: {
            ;
        } break;
        default: {
            __builtin_unreachable();
        } break;
        }
    }
}

void KernelModule::probe() {
    pdevspace->append(new ISA_space{});
    pdevspace->append(new PCI_space{});
}
