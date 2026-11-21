#include "_kmod.hpp"
#include <WM/wm.hpp>

KernelModule *kmod = nullptr;

template<>
size_t hash(const size_t &i) { return i; }

KernelModule::KernelModule():
fs{} {
    kmod = this;
    pdevspace->append(this);
}

void KernelModule::probe() {
    for (auto &head : inside_mod_tranv) {
        reg_insmod(head);
    }
}

void KernelModule::reg_insmod(ModHead &head) {
    switch (head.mod_type) {
    case ModHead::ModType::PDevDrv: {
        auto drv = (PDevDrv &)head;
        switch (drv.drv_type) {
        case PDevDrv::DrvType::ProgIF: {
            pdevspace->append(drv.progid, drv.drv_construc);
        } break;
        case PDevDrv::DrvType::Dev: {
            pdevspace->append(drv.progid, drv.device_type, drv.drv_construc);
        } break;
        default: {
            throw "Bad Module Record";
        } break;
        }
    } break;
    case ModHead::ModType::KrnlObj: {
        ;
    } break;
    case ModHead::ModType::SrvDev: {
        auto srvmod = (SrvDev &)head;
        fs[srvmod.fs_id] = srvmod.fs_construc;
    } break;
    case ModHead::ModType::EnumerMod: {
        auto enumermod = (EnumerMod &)head;
        pdevspace->append(enumermod.mainbody());
    } break;
    case ModHead::ModType::SyscallMod: {
        auto &sysmod = (SyscallMod &)head;
        window_manager->append(sysmod.create(), sysmod.interrupt_index);
    } break;
    default: {
        __builtin_unreachable();
    } break;
    }
}
