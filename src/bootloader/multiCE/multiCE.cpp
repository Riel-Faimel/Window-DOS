#include "_multiCE.hpp"

extern "C" { multiCE Multi_Channel_executor; }

#pragma pack(push, 1)
struct _RZCTX {
    u16 CS;
    u32 EIP;
    u32 EAX;
    u32 ECX;
    u32 EDX;
    u32 EBX;
    u32 ESP;
    u32 EBP;
    u32 ESI;
    u32 EDI;
};
#pragma pack(pop)

multiCE::multiCE() {}

void multiCE::cut(size_t cpuid, size_t tid, size_t intnum) {}

void multiCE::run(void *func, size_t argc, void *argv, u8 ring) {
    for (auto &cpu : rtl::list_tranveser{cpu_list_root}) {
        if (true) {
            cpu.scheduler.run(func, argc, argv, ring);
            return;
        }
    }
}