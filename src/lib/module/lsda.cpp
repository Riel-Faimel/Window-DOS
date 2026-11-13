#include <global/type.hpp>

// LSDA（.gcc_except_table）解析。
// 只做展开器需要的那部分：给定 LSDA、函数起点、当前 PC、抛出类型，
// 返回 landing pad 和 selector；找不到 handler 返回 false。

namespace {

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
    return v;
}

} // namespace

// 从 LSDA 里找覆盖 pc 的 call-site，再走 action 链找 catch。
// catch(...) 在 type table 里是空指针，匹配一切。
extern "C" bool eh_lsda_match(const u8 *lsda, u32 func_start, u32 pc, void *type,
                              u32 &landing, u32 &selector) {
    (void)type;
    if (!lsda) return false;

    const u8 *p = lsda;

    u8 lp_enc = *p++;
    u32 lpstart = func_start;
    if (lp_enc != 0xff) lpstart = lsda_encoded(p, lp_enc);

    u8 tt_enc = *p++;
    u32 tt_off = 0;
    if (tt_enc != 0xff) p = lsda_uleb(p, tt_off);
    (void)tt_enc;
    (void)tt_off;

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

    u32 idx = act;
    while (idx != 0) {
        s32 filter = 0;
        s32 next = 0;

        const u8 *a = action_table;
        for (u32 i = 1; i <= idx; ++i) {
            a = lsda_sleb(a, filter);
            a = lsda_sleb(a, next);
        }

        if (filter > 0) {
            // catch(...) 和带类型的 catch 先都按 handler 处理；
            // type table 的 ttype_offset 语义留到类型匹配时再对。
            landing = lpstart + land;
            selector = (u32)filter;
            return true;
        }

        idx = (u32)next;
    }

    return false;
}
