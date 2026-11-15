#include "_kom.hpp"

KernelObjectManager *kom = nullptr;

KernelObjectManager::KernelObjectManager(): conv_l{} {
    kom = this;
    conv_l.append(new builtin_converter::MBR);
    conv_l.append(new builtin_converter::FileSystem);
}

void KernelObjectManager::append(Converter *conv) {
    if (conv) conv_l.append(conv);
}

void KernelObjectManager::append(KernelObj *obj) {
    if (!obj) return;
    for (auto conv : conv_l) {
        if (conv->touch(obj) != Converter::MatchDegree::Unknow) {
            conv->conv(obj);
            return;
        }
    }
}