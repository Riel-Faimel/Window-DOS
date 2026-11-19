#include <global/type.hpp>

// LSDA（.gcc_except_table）解析。
// 给定 LSDA、函数起点、当前 PC、抛出类型，返回 landing pad、selector
// 以及 catch 参数相对抛出对象的字节偏移。

extern "C" bool __eh_type_match(const void *thrown, const void *catch_type, s32 *offset);

namespace {

constexpr u8 EH_PE_pcrel = 0x10;
constexpr u8 EH_PE_indirect = 0x80;

const u8 *lsda_uleb(const u8 *p, u32 &v) {
    u32 r = 0, s = 0;
    for (;;) {
        u8 b = *p++;
        r |= (u32)(b & 0x7f) << s;
        if (!(b & 0x80)) break;
        s += 7;
    }
    v = r;
    return p;
}

const u8 *lsda_sleb(const u8 *p, s32 &v) {
    s32 r = 0;
    u32 s = 0;
    u8 b;
    do {
        b = *p++;
        r |= (s32)(b & 0x7f) << s;
        s += 7;
    } while (b & 0x80);
    if (s < 32 && (b & 0x40)) r |= -(1 << s);
    v = r;
    return p;
}

u32 lsda_encoded(const u8 *&p, u8 enc) {
    const u8 *field = p;
    u32 v = 0;
    switch (enc & 0x0f) {
    case 0x00: v = *(const u32 *)p; p += 4; break;
    case 0x01: p = lsda_uleb(p, v); break;
    case 0x02: v = *(const u16 *)p; p += 2; break;
    case 0x03: v = *(const u32 *)p; p += 4; break;
    case 0x04: v = *(const u32 *)p; p += 8; break;
    case 0x09: { s32 t; p = lsda_sleb(p, t); v = (u32)t; } break;
    case 0x0a: v = (u32)(s32)(s16)*(const u16 *)p; p += 2; break;
    case 0x0b: v = (u32)(s32)*(const u32 *)p; p += 4; break;
    case 0x0c: v = *(const u32 *)p; p += 8; break;
    default: break;
    }
    if (enc & EH_PE_pcrel) v += (u32)field;
    if (enc & EH_PE_indirect) v = *(const u32 *)v;
    return v;
}

u32 type_entry_size(u8 enc) {
    switch (enc & 0x0f) {
    case 0x02: case 0x0a: return 2;
    case 0x04: case 0x0c: return 8;
    default: return 4;
    }
}

} // namespace

// 从 LSDA 里找覆盖 pc 的 call-site，再走 action 链找第一个匹配的 catch。
// 类型表项是 catch 类型的 _ZTI 指针；catch(...) 的项是空指针。
// TType 指向类型表的末尾，第 filter 项在 TType - filter * entry_size。
extern "C" bool eh_lsda_match(const u8 *lsda, u32 func_start, u32 pc, void *type,
                              u32 &landing, u32 &selector, s32 &offset) {
    if (!lsda) return false;

    const u8 *p = lsda;

    u8 lp_enc = *p++;
    u32 lpstart = func_start;
    if (lp_enc != 0xff) lpstart = lsda_encoded(p, lp_enc);

    u8 tt_enc = *p++;
    const u8 *tt_end = nullptr;
    if (tt_enc != 0xff) {
        u32 tt_off = 0;
        p = lsda_uleb(p, tt_off);
        tt_end = p + tt_off;
    }

    u8 cs_enc = *p++;
    u32 cs_len;
    p = lsda_uleb(p, cs_len);

    const u8 *cs = p;
    const u8 *cs_end = p + cs_len;
    const u8 *action_table = cs_end;

    u32 off = pc - lpstart;
    u32 land = 0;
    u32 act = 0;

    for (const u8 *q = cs; q < cs_end; ) {
        u32 start = lsda_encoded(q, cs_enc);
        u32 len = lsda_encoded(q, cs_enc);
        u32 l = lsda_encoded(q, cs_enc);
        u32 a = lsda_encoded(q, cs_enc);
        if (off >= start && off < start + len) {
            land = l;
            act = a;
            break;
        }
    }

    if (land == 0 || act == 0) return false;

    const u8 *action_record = action_table + (act - 1);
    while (true) {
        s32 filter = 0;
        s32 next = 0;

        const u8 *a = action_record;
        a = lsda_sleb(a, filter);
        lsda_sleb(a, next);

        // filter > 0：带类型的 catch 或 catch(...)；filter == 0 是 cleanup，
        // filter < 0 是异常规格，两者都不参与类型匹配。
        if (filter > 0) {
            u32 catch_ti = 0;
            if (tt_end) {
                const u8 *e = tt_end - (u32)filter * type_entry_size(tt_enc);
                catch_ti = lsda_encoded(e, tt_enc);
            }

            s32 adjust = 0;
            if (__eh_type_match(type, (const void *)catch_ti, &adjust)) {
                landing = lpstart + land;
                selector = (u32)filter;
                offset = adjust;
                return true;
            }
        }

        if (next == 0) break;
        action_record = a + next;
    }

    return false;
}
