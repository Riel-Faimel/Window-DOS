#pragma once

#include "svm.hpp"
#include <interface/svm/data.hpp>
#include <stdio>
#include <interface/knlobj/knlobj.hpp>
#include <KrnlObjMgr/kom.hpp>
#include <SrvDevMgr/DLS.hpp>
#include <mm/mm.hpp>

// Executor context: SysVMThread keeps its state private, so the operation
// functions (anonymous namespace in vmop.cpp) receive references to exactly
// the pieces they are allowed to touch. Output result descriptors live in
// SysVMThread::stack slots, not in thread fields.
struct OpCtx {
    SysVMThread &th;
    svm::Value *stack;
    unsigned &sp;
};

constexpr unsigned OpcodeCount =
    static_cast<unsigned>(svm::Op::Status) + 1;

// Result descriptors are carved out of SysVMThread::stack. A token Value
// precedes each descriptor so reset() can find and free the descriptor's
// heap array without any thread-side bookkeeping.
constexpr u32 ResultTokenMagic = 0xFFFF0000u;
constexpr unsigned ResultKindShift = 8;
constexpr unsigned ResultSlotsMask = 0xFFu;

enum class ResultKind : unsigned {
    Read = 1,
    Mem = 2,
    Obj = 3,
    Fs = 4,
    Status = 5,
};

extern bool (*Opcode2Func[OpcodeCount])(OpCtx &, const svm::Insn &);
