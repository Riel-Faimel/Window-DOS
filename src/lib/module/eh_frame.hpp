#ifndef __lib_module_eh_frame_hpp
#define __lib_module_eh_frame_hpp

#include <global/type.hpp>

// 展开器与 CFI 状态：.eh_frame 解析、__cxa_*、LSDA 匹配共用。

enum EhRule : u8 {
    EH_SAME = 0,
    EH_UNDEFINED,
    EH_OFFSET,
    EH_VAL_OFFSET,
    EH_REGISTER,
};

struct EhRegRule {
    u8 how;
    s32 offset;
    u8 reg;
};

struct EhCfiState {
    u8 cfa_reg;
    s32 cfa_offset;
    EhRegRule rules[9];
};

struct EhContext {
    u32 esp;
    u32 ebp;
    u32 ebx;
    u32 esi;
    u32 edi;
    u32 eip;

    u32 value(u8 reg) const {
        switch (reg) {
        case 3: return ebx;
        case 4: return esp;
        case 5: return ebp;
        case 6: return esi;
        case 7: return edi;
        case 8: return eip;
        default: return 0;
        }
    }

    u32 rule(const EhCfiState &state, u8 reg, u32 cfa) const {
        if (reg >= 9) return 0;
        const EhRegRule &r = state.rules[reg];
        switch (r.how) {
        case EH_OFFSET: return *(const u32 *)(cfa + r.offset);
        case EH_VAL_OFFSET: return cfa + r.offset;
        case EH_REGISTER: return value(r.reg);
        case EH_UNDEFINED: return 0;
        case EH_SAME:
        default: return value(reg);
        }
    }
};

extern "C" void eh_capture(void *);
bool eh_unwind_step(EhContext &);
void eh_install(EhContext *);
bool eh_lookup(u32 pc, u32 &lsda, u32 &func);

// LSDA 的 catch 类型匹配。offset 是 catch 参数相对抛出对象的字节偏移，
// catch(...) 时 type 为空、offset 为 0。
extern "C" bool eh_lsda_match(const u8 *lsda, u32 func_start, u32 pc, void *type,
                              u32 &landing, u32 &selector, s32 &offset);

extern void *eh_thrown_type;
extern "C" u32 eh_landing, eh_exc, eh_sel;

void eh_backtrace();

#endif
