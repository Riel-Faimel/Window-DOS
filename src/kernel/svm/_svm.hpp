#include "svm.hpp"
#include <stdio>
#include <interface/knlobj/knlobj.hpp>
#include <KrnlObjMgr/kom.hpp>
#include <SrvDevMgr/DLS.hpp>

namespace {

struct DiskPick {
    unsigned want = 0;
    unsigned seen = 0;
    PhysicalDisk *disk = nullptr;
};

void pick_disk(KernelObj *o, void *ctx) {
    auto *p = static_cast<DiskPick *>(ctx);
    auto *disk = dynamic_cast<PhysicalDisk *>(o);
    if (!disk) return;
    if (p->seen++ == p->want) p->disk = disk;
}

struct ObjWriter {
    svm::OutBuf *out;
};

void write_obj(KernelObj *o, void *ctx) {
    auto *w = static_cast<ObjWriter *>(ctx);
    svm::ObjRec rec{};

    if (auto *p = dynamic_cast<PhysicalDisk *>(o)) {
        rec.kind = svm::RecKind::Phys;
        rec.a = reinterpret_cast<u32>(p);
    } else if (auto *d = dynamic_cast<LogicalDisk *>(o)) {
        auto *i = d->info();
        rec.kind = svm::RecKind::Part;
        rec.a = static_cast<u32>(i->su_start);
        rec.b = i->su_num.low;
        rec.c = static_cast<u32>(i->type);
    } else if (dynamic_cast<SerialPort *>(o)) {
        rec.kind = svm::RecKind::Serial;
    } else if (dynamic_cast<FramePort *>(o)) {
        rec.kind = svm::RecKind::Frame;
    } else {
        rec.kind = svm::RecKind::Unknown;
    }

    w->out->put_as(rec);
}

} // namespace