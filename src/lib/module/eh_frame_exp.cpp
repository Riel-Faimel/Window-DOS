#include "eh_frame.hpp"
#include <stdio>

extern "C" {
extern void *__eh_frame_start __asm__("__eh_frame_start");
extern void *__eh_frame_end __asm__("__eh_frame_end");
}

// .eh_frame 的解析与展开。只做展开器需要的那部分：
// 给定 PC 找到 FDE，执行 CFI 得到上一帧的寄存器。

constexpr u8 EH_PE_absptr = 0x00;
constexpr u8 EH_PE_uleb128 = 0x01;
constexpr u8 EH_PE_udata2 = 0x02;
constexpr u8 EH_PE_udata4 = 0x03;
constexpr u8 EH_PE_udata8 = 0x04;
constexpr u8 EH_PE_sleb128 = 0x09;
constexpr u8 EH_PE_sdata2 = 0x0a;
constexpr u8 EH_PE_sdata4 = 0x0b;
constexpr u8 EH_PE_sdata8 = 0x0c;
constexpr u8 EH_PE_pcrel = 0x10;
constexpr u8 EH_PE_indirect = 0x80;

struct EhFrameHead {
    u32 length;
    u32 id;

protected:
    static u32 load_u32(const u8 *p) { return *(const u32 *)p; }
    static u16 load_u16(const u8 *p) { return *(const u16 *)p; }

    static const u8 *read_uleb(const u8 *p, u32 &value) {
        u32 result = 0;
        u32 shift = 0;
        for (;;) {
            u8 byte = *p++;
            result |= (u32)(byte & 0x7f) << shift;
            if (!(byte & 0x80)) break;
            shift += 7;
        }
        value = result;
        return p;
    }

    static const u8 *read_sleb(const u8 *p, s32 &value) {
        s32 result = 0;
        u32 shift = 0;
        u8 byte;
        do {
            byte = *p++;
            result |= (s32)(byte & 0x7f) << shift;
            shift += 7;
        } while (byte & 0x80);
        if (shift < 32 && (byte & 0x40)) result |= -(1 << shift);
        value = result;
        return p;
    }

    static u32 read_encoded(const u8 *&p, u8 encoding) {
        const u8 *field = p;
        u32 value = 0;

        switch (encoding & 0x0f) {
        case EH_PE_absptr: value = load_u32(p); p += 4; break;
        case EH_PE_uleb128: p = read_uleb(p, value); break;
        case EH_PE_udata2: value = load_u16(p); p += 2; break;
        case EH_PE_udata4: value = load_u32(p); p += 4; break;
        case EH_PE_udata8: value = load_u32(p); p += 8; break;
        case EH_PE_sleb128: { s32 tmp; p = read_sleb(p, tmp); value = (u32)tmp; } break;
        case EH_PE_sdata2: value = (u32)(s32)(s16)load_u16(p); p += 2; break;
        case EH_PE_sdata4: value = (u32)(s32)load_u32(p); p += 4; break;
        case EH_PE_sdata8: value = load_u32(p); p += 8; break;
        default: break;
        }

        if (encoding & EH_PE_pcrel) value += (u32)field;
        if (encoding & EH_PE_indirect) value = load_u32((const u8 *)value);
        return value;
    }
};

// CIE：aug[] 是变长字符串，所以布局只能到 aug；aug 之后的解析不能作为字段，
// 但也不散落到全局，全部是 CIE 的成员函数。
class EhFrameCIE : EhFrameHead {
public:
    u8 version;
    char aug[];

    const u8 *end() const { return (const u8 *)this + 4 + length; }

    bool has_z() const { return aug[0] == 'z'; }

    // 解析 aug 之后的 align / ra / aug data，返回 CFI 起点
    const u8 *scan(u32 &code_align, s32 &data_align, u8 &ra_reg,
                   u8 &fde_encoding, u8 &lsda_encoding, bool &has_lsda) const {
        const u8 *p = (const u8 *)aug;
        while (*p) ++p;
        ++p;

        p = read_uleb(p, code_align);
        p = read_sleb(p, data_align);

        u32 ra;
        p = read_uleb(p, ra);
        ra_reg = (u8)ra;

        fde_encoding = EH_PE_absptr;
        lsda_encoding = 0;
        has_lsda = false;

        if (has_z()) {
            u32 n;
            p = read_uleb(p, n);
            const u8 *aug_end = p + n;

            for (auto a = aug + 1; *a; ++a) {
                switch (*a) {
                case 'R': fde_encoding = *p++; break;
                case 'L': lsda_encoding = *p++; has_lsda = true; break;
                case 'P': { u8 enc = *p++; read_encoded(p, enc); } break;
                case 'S': break;
                }
            }
            p = aug_end;
        }
        return p;
    }
};

// FDE：没有变长字符串，所以可以像 PDevDrv 一样带字段。
// base 指向原记录；parse() 把 CIE 之后的字段解析到成员里。
class EhFrameFDE : EhFrameHead {
    const u8 *base;
    const EhFrameCIE *cie_obj;
    const u8 *cie_insn;
    const u8 *insn;
    u32 initial_location;
    u32 range;
    u32 lsda;
    u32 code_align;
    s32 data_align;
    u8 ra_reg;

public:
    EhFrameFDE(const EhFrameHead &head):
    base{(const u8 *)&head},
    cie_obj((const EhFrameCIE *)((const u8 *)&head + 4 - head.id)) {
        length = head.length;
        id = head.id;
    }

    const u8 *end() const { return base + 4 + length; }
    const EhFrameCIE &cie() const { return *cie_obj; }

    u32 start() const { return initial_location; }
    u32 size() const { return range; }
    const u8 *lsda_ptr() const { return (const u8 *)lsda; }

    bool parse() {
        u8 fde_encoding, lsda_encoding;
        bool has_lsda;
        cie_insn = cie_obj->scan(code_align, data_align, ra_reg,
                                fde_encoding, lsda_encoding, has_lsda);

        const u8 *p = base + 8;
        initial_location = read_encoded(p, fde_encoding);
        range = read_encoded(p, (u8)(fde_encoding & 0x0f));

        lsda = 0;
        if (cie_obj->has_z()) {
            u32 n;
            p = read_uleb(p, n);
            const u8 *aug_end = p + n;
            if (has_lsda) lsda = read_encoded(p, lsda_encoding);
            p = aug_end;
        }
        insn = p;
        return insn <= end();
    }

    bool unwind(EhContext &ctx) const {
        if (ctx.eip == 0) return false;

        u32 pc = ctx.eip - 1;
        if (pc < initial_location || pc >= initial_location + range) return false;

        EhCfiState state{};
        state.cfa_reg = 4;   // ESP
        state.cfa_offset = 4;

        exec_cfi(cie_insn, cie_obj->end(), 0, 0, code_align, data_align, state);
        exec_cfi(insn, end(), initial_location, pc, code_align, data_align, state);

        u32 cfa = ctx.value(state.cfa_reg) + (u32)state.cfa_offset;

        u32 next_eip = ctx.rule(state, ra_reg, cfa);
        u32 next_ebp = ctx.rule(state, 5, cfa);
        u32 next_ebx = ctx.rule(state, 3, cfa);
        u32 next_esi = ctx.rule(state, 6, cfa);
        u32 next_edi = ctx.rule(state, 7, cfa);

        if (next_eip == 0) return false;

        ctx.esp = cfa;
        ctx.eip = next_eip;
        ctx.ebp = next_ebp;
        ctx.ebx = next_ebx;
        ctx.esi = next_esi;
        ctx.edi = next_edi;
        return true;
    }

private:
    static void set_offset(EhCfiState &state, u32 reg, s32 units, s32 data_align) {
        if (reg >= 9) return;
        state.rules[reg].how = EH_OFFSET;
        state.rules[reg].offset = units * data_align;
    }

    static const u8 *skip_block(const u8 *p) {
        u32 len;
        p = read_uleb(p, len);
        return p + len;
    }

    static void exec_cfi(const u8 *p, const u8 *end, u32 start_loc, u32 target_pc,
                         u32 code_align, s32 data_align, EhCfiState &state) {
        u32 loc = start_loc;

        while (p < end) {
            u8 op = *p++;

            if (op == 0) continue;

            if (op >= 0xc0) continue; // restore（no-op：throw 点不会在 epilogue）
            if (op >= 0x80) { // offset
                u32 reg = op & 0x3f;
                u32 units;
                p = read_uleb(p, units);
                set_offset(state, reg, (s32)units, data_align);
                continue;
            }
            if (op >= 0x40) { // advance_loc
                u32 delta = (u32)(op & 0x3f) * code_align;
                if (loc + delta > target_pc) return;
                loc += delta;
                continue;
            }

            switch (op) {
            case 0x01: { u32 v; p = read_uleb(p, v); if (v > target_pc) return; loc = v; } break;
            case 0x02: { u32 d = *p++; if (loc + d > target_pc) return; loc += d; } break;
            case 0x03: { u32 d = load_u16(p); p += 2; if (loc + d > target_pc) return; loc += d; } break;
            case 0x04: { u32 d = load_u32(p); p += 4; if (loc + d > target_pc) return; loc += d; } break;
            case 0x05: {
                u32 reg, units;
                p = read_uleb(p, reg);
                p = read_uleb(p, units);
                set_offset(state, reg, (s32)units, data_align);
            } break;
            case 0x06: { u32 reg; p = read_uleb(p, reg); } break;
            case 0x07: { u32 reg; p = read_uleb(p, reg); if (reg < 9) state.rules[reg].how = EH_UNDEFINED; } break;
            case 0x08: { u32 reg; p = read_uleb(p, reg); if (reg < 9) state.rules[reg].how = EH_SAME; } break;
            case 0x09: {
                u32 reg, reg2;
                p = read_uleb(p, reg);
                p = read_uleb(p, reg2);
                if (reg < 9) { state.rules[reg].how = EH_REGISTER; state.rules[reg].reg = (u8)reg2; }
            } break;
            case 0x0a: break;
            case 0x0b: break;
            case 0x0c: {
                u32 reg, off;
                p = read_uleb(p, reg);
                p = read_uleb(p, off);
                state.cfa_reg = (u8)reg;
                state.cfa_offset = (s32)off;
            } break;
            case 0x0d: { u32 reg; p = read_uleb(p, reg); state.cfa_reg = (u8)reg; } break;
            case 0x0e: { u32 off; p = read_uleb(p, off); state.cfa_offset = (s32)off; } break;
            case 0x0f: p = skip_block(p); break;
            case 0x10: { u32 reg; p = read_uleb(p, reg); p = skip_block(p); } break;
            case 0x11: {
                u32 reg; s32 units;
                p = read_uleb(p, reg);
                p = read_sleb(p, units);
                set_offset(state, reg, units, data_align);
            } break;
            case 0x12: {
                u32 reg; s32 off;
                p = read_uleb(p, reg);
                p = read_sleb(p, off);
                state.cfa_reg = (u8)reg;
                state.cfa_offset = off * data_align;
            } break;
            case 0x13: { s32 off; p = read_sleb(p, off); state.cfa_offset = off * data_align; } break;
            case 0x14: {
                u32 reg, units;
                p = read_uleb(p, reg);
                p = read_uleb(p, units);
                if (reg < 9) { state.rules[reg].how = EH_VAL_OFFSET; state.rules[reg].offset = (s32)units * data_align; }
            } break;
            case 0x15: {
                u32 reg; s32 units;
                p = read_uleb(p, reg);
                p = read_sleb(p, units);
                if (reg < 9) { state.rules[reg].how = EH_VAL_OFFSET; state.rules[reg].offset = units * data_align; }
            } break;
            case 0x16: { u32 reg; p = read_uleb(p, reg); p = skip_block(p); } break;
            case 0x2e: { u32 size; p = read_uleb(p, size); } break;
            default: return;
            }
        }
    }
};

namespace {

class {
public:
    class iterator {
        EhFrameHead *ptr = (EhFrameHead *)&__eh_frame_start;
    public:
        EhFrameHead &operator * () const { return *ptr; }

        iterator &operator ++ () {
            if (ptr->length == 0) { ptr = (EhFrameHead *)&__eh_frame_end; return *this; }
            ptr = (EhFrameHead *)((const u8 *)ptr + 4 + ptr->length);
            return *this;
        }

        bool operator != (const iterator &) const {
            return ptr < (EhFrameHead *)&__eh_frame_end;
        }
    };

    auto begin() const { return iterator{}; }
    auto end() const { return iterator{}; }
} eh_frame_tranv;

}

bool eh_lookup(u32 pc, u32 &lsda, u32 &func) {
    for (auto &record : eh_frame_tranv) {
        if (!record.id) continue;

        EhFrameFDE fde(record);
        if (!fde.parse()) continue;
        if (pc < fde.start() || pc >= fde.start() + fde.size()) continue;

        lsda = (u32)fde.lsda_ptr();
        func = fde.start();
        return true;
    }
    return false;
}

bool eh_unwind_step(EhContext &ctx) {
    for (auto &record : eh_frame_tranv) {
        if (!record.id) continue;

        EhFrameFDE fde(record);
        if (!fde.parse()) continue;
        if (fde.unwind(ctx)) return true;
    }
    return false;
}

void eh_backtrace() {
    EhContext ctx;
    eh_capture(&ctx);

    kout << "[EH] backtrace:\n";
    for (u32 i = 0; i < 10; ++i) {
        kout << "  #" << (char)('0' + i) << "  " << (void *)ctx.eip << '\n';
        if (!eh_unwind_step(ctx)) break;
    }
}
