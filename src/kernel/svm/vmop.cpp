#include "_svm.hpp"

using namespace svm;

namespace {

// Numeric selector carried by Op::Status. Names belong to the frontend; SVM
// only knows the numeric module ids.
enum class StatusSel : u32 {
    All = 0,
    Mm,
    Page,
    Obj,
    Fs,
};

// Result descriptors live in SVM value-stack slots. The descriptor is stored
// first, then the Value that points at it is pushed on top. The descriptor
// slots are reclaimed by SysVMThread::reset() (sp = 0).
template <typename T>
T *alloc_result(OpCtx &ctx, ResultKind kind) {
    static_assert(alignof(T) <= alignof(Value));

    constexpr unsigned slots =
        (sizeof(T) + sizeof(Value) - 1) / sizeof(Value);
    if (ctx.sp + 1 + slots + 1 > SysVMThread::stack_max) return nullptr;

    ctx.stack[ctx.sp++] = Value{
        Value::TypeID::Reg,
        ResultTokenMagic |
            (static_cast<unsigned>(kind) << ResultKindShift) |
            slots
    };

    auto *p = reinterpret_cast<T *>(&ctx.stack[ctx.sp]);
    ctx.sp += slots;
    return p;
}

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

void count_obj(KernelObj *, void *ctx) {
    ++*static_cast<size_t *>(ctx);
}

struct ObjWriter {
    ObjEntry *addr;
    size_t i = 0;
};

void write_obj(KernelObj *o, void *ctx) {
    auto *w = static_cast<ObjWriter *>(ctx);
    ObjEntry rec{};

    if (auto *p = dynamic_cast<PhysicalDisk *>(o)) {
        rec.kind = ObjKind::Phys;
        rec.a = reinterpret_cast<u32>(p);
    } else if (auto *d = dynamic_cast<LogicalDisk *>(o)) {
        auto *i = d->info();
        if (i) {
            rec.kind = ObjKind::Part;
            rec.a = static_cast<u32>(i->su_start);
            rec.b = i->su_num.low;
            rec.c = static_cast<u32>(i->type);
        } else {
            rec.kind = ObjKind::Unknown;
            rec.a = reinterpret_cast<u32>(d);
        }
    } else if (dynamic_cast<SerialPort *>(o)) {
        rec.kind = ObjKind::Serial;
    } else if (dynamic_cast<FramePort *>(o)) {
        rec.kind = ObjKind::Frame;
    } else {
        rec.kind = ObjKind::Unknown;
        rec.a = reinterpret_cast<u32>(o);
    }

    w->addr[w->i++] = rec;
}

bool build_objs(ObjEntry *&addr, size_t &len) {
    size_t count = 0;
    kom->visit(count_obj, &count);

    addr = nullptr;
    len = count;
    if (!count) return true;

    addr = new ObjEntry[count];
    if (!addr) return false;

    ObjWriter w{addr};
    kom->visit(write_obj, &w);
    return true;
}

bool build_drives(DriveEntry *&addr, size_t &len) {
    const size_t count = driverletterspace
        ? driverletterspace->space.get_size()
        : 0;

    addr = nullptr;
    len = count;
    if (!count) return true;

    addr = new DriveEntry[count];
    if (!addr) return false;

    for (size_t i = 0; i < count; i++) {
        const auto &d = driverletterspace->space[i];
        addr[i] = DriveEntry{
            d.driver_letter.empty()
                ? 0u
                : static_cast<u32>(d.driver_letter.c_str()[0]),
            d.cid
        };
    }
    return true;
}

// Push a literal, or the contents of a register.
bool op_push(OpCtx &ctx, const Insn &in) {
    if (in.imm.type == Value::TypeID::Reg) {
        Value *p = ctx.th.slot_of(in.imm);
        if (!p) return false;
        ctx.th.push(*p);
    } else {
        ctx.th.push(in.imm);
    }
    return true;
}

// Assign: imm points at a stack slot / register; the value is the stack top.
bool op_assign(OpCtx &ctx, const Insn &in) {
    Value *target = ctx.th.slot_of(in.imm);
    if (!target || !ctx.sp) return false;

    const Value top = ctx.th.pop();
    if (target->type != Value::TypeID::Nil && target->type != top.type)
        return false;
    *target = top;
    return true;
}

bool op_drop(OpCtx &ctx, const Insn &) {
    if (!ctx.sp) return false;
    ctx.sp--;
    return true;
}

bool op_swap(OpCtx &ctx, const Insn &) {
    if (ctx.sp < 2) return false;

    Value &a = ctx.stack[ctx.sp - 1];
    Value &b = ctx.stack[ctx.sp - 2];
    if (a.type != b.type) return false;

    const Value t = a;
    a = b;
    b = t;
    return true;
}

// Compare: the left side is the stack top, the right side is imm. Both are
// dereferenced through Ref/Reg. Only strongly ordered types are accepted.
bool op_cmp(OpCtx &ctx, const Insn &in) {
    if (!ctx.sp) return false;

    const Value a = ctx.th.value_of(ctx.stack[ctx.sp - 1]);
    const Value b = ctx.th.value_of(in.imm);
    if (a.type != b.type) return false;

    s32 r;
    switch (a.type) {
    case Value::TypeID::Int: {
        const s32 x = static_cast<s32>(a.value);
        const s32 y = static_cast<s32>(b.value);
        r = (x < y) ? -1 : (x > y) ? 1 : 0;
        break;
    }
    case Value::TypeID::Obj:
    case Value::TypeID::Ref: {
        r = (a.value < b.value) ? -1 : (a.value > b.value) ? 1 : 0;
        break;
    }
    default:
        return false;
    }

    ctx.sp--;
    ctx.th.push(Value{Value::TypeID::Ordering, static_cast<u32>(r)});
    return true;
}

// Disk number -> object pointer in the target slot.
bool op_open(OpCtx &ctx, const Insn &in) {
    if (!ctx.sp ||
        ctx.stack[ctx.sp - 1].type != Value::TypeID::Int)
        return false;

    Value *target = ctx.th.slot_of(in.imm);
    if (!target) return false;

    const unsigned idx = ctx.th.pop().value;

    DiskPick pick;
    pick.want = idx;
    kom->visit(pick_disk, &pick);
    if (!pick.disk) {
        *target = Value{Value::TypeID::Nil, 0};
        return true;
    }

    *target = Value{
        Value::TypeID::Obj,
        reinterpret_cast<u32>(pick.disk)
    };
    return true;
}

// Stack: disk(Ref), lba(Int), count(Int). Result: ReadResult* on the VM stack.
bool op_read(OpCtx &ctx, const Insn &in) {
    (void)in;
    if (ctx.sp < 3) return false;
    if (ctx.stack[ctx.sp - 1].type != Value::TypeID::Int) return false;
    if (ctx.stack[ctx.sp - 2].type != Value::TypeID::Int) return false;
    if (ctx.stack[ctx.sp - 3].type != Value::TypeID::Ref) return false;

    const unsigned cnt = ctx.th.pop().value;
    const unsigned lba = ctx.th.pop().value;

    const unsigned slot = ctx.th.pop().value;
    if (slot >= SysVMThread::stack_max) return false;

    Value &obj = ctx.stack[slot];
    if (obj.type != Value::TypeID::Obj) return false;
    auto *disk = reinterpret_cast<PhysicalDisk *>(obj.value);
    if (!disk) return false;

    const size_t need = static_cast<size_t>(cnt) * 512u;
    auto *addr = new u8[need];
    if (!addr) return false;

    const unsigned got = disk->read(addr, lba, 0, cnt);
    if (got == (unsigned)-1) {
        delete[] addr;
        return false;
    }

    auto *res = alloc_result<ReadResult>(ctx, ResultKind::Read);
    if (!res) {
        delete[] addr;
        return false;
    }
    res->addr = addr;
    res->len = got;

    ctx.th.push(Value{
        Value::TypeID::ReadOut,
        reinterpret_cast<u32>(res)
    });
    return true;
}

// Stack: addr(Int), count(Int). Result: MemResult* on the VM stack.
bool op_mem(OpCtx &ctx, const Insn &in) {
    (void)in;
    if (ctx.sp < 2) return false;
    if (ctx.stack[ctx.sp - 1].type != Value::TypeID::Int) return false;
    if (ctx.stack[ctx.sp - 2].type != Value::TypeID::Int) return false;

    const unsigned cnt = ctx.th.pop().value;
    const unsigned mem_addr = ctx.th.pop().value;

    auto *addr = new u8[cnt];
    if (cnt && !addr) return false;

    const auto *src = reinterpret_cast<const u8 *>(mem_addr);
    for (unsigned i = 0; i < cnt; i++) addr[i] = src[i];

    auto *res = alloc_result<MemResult>(ctx, ResultKind::Mem);
    if (!res) {
        delete[] addr;
        return false;
    }
    res->addr = addr;
    res->len = cnt;

    ctx.th.push(Value{
        Value::TypeID::MemOut,
        reinterpret_cast<u32>(res)
    });
    return true;
}

// No operands. Result: ObjResult* on the VM stack.
bool op_obj(OpCtx &ctx, const Insn &in) {
    (void)in;

    ObjEntry *addr = nullptr;
    size_t len = 0;
    if (!build_objs(addr, len)) return false;

    auto *res = alloc_result<ObjResult>(ctx, ResultKind::Obj);
    if (!res) {
        delete[] addr;
        return false;
    }
    res->addr = addr;
    res->len = len;

    ctx.th.push(Value{
        Value::TypeID::ObjOut,
        reinterpret_cast<u32>(res)
    });
    return true;
}

// No operands. Result: FsResult* on the VM stack.
bool op_fs(OpCtx &ctx, const Insn &in) {
    (void)in;

    DriveEntry *addr = nullptr;
    size_t len = 0;
    if (!build_drives(addr, len)) return false;

    auto *res = alloc_result<FsResult>(ctx, ResultKind::Fs);
    if (!res) {
        delete[] addr;
        return false;
    }
    res->addr = addr;
    res->len = len;

    ctx.th.push(Value{
        Value::TypeID::FsOut,
        reinterpret_cast<u32>(res)
    });
    return true;
}

// Stack: module id(Int). Result: StatusResult* on the VM stack.
bool op_status(OpCtx &ctx, const Insn &in) {
    (void)in;
    if (!ctx.sp || ctx.stack[ctx.sp - 1].type != Value::TypeID::Int)
        return false;

    const u32 selector = ctx.th.pop().value;
    if (selector > static_cast<u32>(StatusSel::Fs)) return false;

    const bool all = selector == static_cast<u32>(StatusSel::All);
    ObjEntry *objs = nullptr;
    size_t obj_len = 0;
    DriveEntry *drives = nullptr;
    size_t fs_len = 0;

    if ((all || selector == static_cast<u32>(StatusSel::Obj)) &&
        !build_objs(objs, obj_len))
        return false;

    if ((all || selector == static_cast<u32>(StatusSel::Fs)) &&
        !build_drives(drives, fs_len)) {
        delete[] objs;
        return false;
    }

    auto *res = alloc_result<StatusResult>(ctx, ResultKind::Status);
    if (!res) {
        delete[] objs;
        delete[] drives;
        return false;
    }

    *res = StatusResult{};
    res->selector = selector;
    res->objs = objs;
    res->obj_len = obj_len;
    res->drives = drives;
    res->fs_len = fs_len;

    if (all || selector == static_cast<u32>(StatusSel::Mm)) {
        auto m = global_heap
            ? global_heap->status()
            : MemoryManager::MemStatus{};
        res->total_mem = m.total_mem;
        res->alloced_mem = m.alloced_mem;
        res->used_mem = m.used_mem;
        res->free_mem = m.free_mem;
    }

    if (all || selector == static_cast<u32>(StatusSel::Page)) {
        const auto p = physicalpage.status();
        res->page_total = p.total_mem;
        res->page_free = p.free_mem;
    }

    ctx.th.push(Value{
        Value::TypeID::StatusOut,
        reinterpret_cast<u32>(res)
    });
    return true;
}

} // namespace

bool (*Opcode2Func[OpcodeCount])(OpCtx &, const svm::Insn &) = {
    nullptr,     // None
    op_push,     // Push
    op_assign,   // Assign
    op_drop,     // Drop
    op_swap,     // Swap
    op_cmp,      // Cmp
    nullptr,     // Jmp
    nullptr,     // Jc
    op_open,     // Open
    op_read,     // Read
    op_mem,      // Mem
    op_obj,      // Obj
    op_fs,       // Fs
    op_status,   // Status
};
