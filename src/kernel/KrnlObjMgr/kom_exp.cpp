#include "_kom.hpp"

KernelObjectManager *kom = nullptr;

KernelObjectManager::KernelObjectManager(): conv_l{}, obj_l{} {
    kom = this;
    conv_l.append(new builtin_converter::MBR);
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

    rtl::array<Converter *> match;
    rtl::array<Converter *> matchbad;
    rtl::array<Converter *> work;
    for (auto conv : conv_l) {
        switch (conv->touch(obj)) {
        case Converter::MatchDegree::MatchWell: {
            conv->conv(obj);
            return ObjID{id};
        } break;
        case Converter::MatchDegree::Match: {
            match.append(conv);
        } break;
        case Converter::MatchDegree::MatchBad: {
            matchbad.append(conv);
        } break;
        case Converter::MatchDegree::Work: {
            work.append(conv);
        } break;
        case Converter::MatchDegree::Unknow: {
            continue;
        } break;
        default: {
            continue;
        } break;
        }
    }
    // emmm...
    for (auto conv : match) {
        conv->conv(obj);
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
