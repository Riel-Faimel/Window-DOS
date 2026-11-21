#include "_kom.hpp"

KernelObjectManager *kom = nullptr;

KernelObjectManager::KernelObjectManager(): conv_l{}, obj_l{} {
    kom = this;
    conv_l.append(new builtin_converter::MBR);
    conv_l.append(new builtin_converter::FileSystem);
}

void KernelObjectManager::append(Converter *conv) {
    if (conv) conv_l.append(conv);
}

KernelObjectManager::ObjID KernelObjectManager::append(KernelObj *obj) {
    if (!obj) return ObjID{0};

    for (auto &entry : obj_l)
    if (entry.obj == obj)
    return entry.id;

    const u32 id = next_id++;
    obj_l.append(Entry{ObjID{id}, obj});

    for (auto conv : conv_l) {
        if (conv->touch(obj) != Converter::MatchDegree::Unknow) {
            conv->conv(obj);
            break;
        }
    }
    return ObjID{id};
}

bool KernelObjectManager::delet_(ObjID id) {
    if (!id.valid()) return false;

    for (size_t i = 0; i < obj_l.get_size(); i++) {
        if (!(obj_l[i].id == id)) continue;
        obj_l.remove(i);
        return true;
    }
    return false;
}

void KernelObjectManager::visit(void (*fn)(KernelObj *, void *), void *ctx) {
    if (!fn) return;
    for (auto &entry : obj_l)
        if (entry.obj) fn(entry.obj, ctx);
}
