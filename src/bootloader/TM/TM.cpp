#include "_TM.hpp"

void caller(size_t n, void *v) {
    void *start_point = *(void **)v;
    void *argv = *((void **)v + 1);
    asm volatile (
        "mov %0, %%eax\n"
        "mov %1, %%edx\n"
        "call %2\n"
        "int $0x31\n" // process exit
        "int $0x32\n" // thread exit
        :
        : "r"(n), "r"(argv), "r"(start_point)
        : "eax", "edx", "memory"
    );
}
void hello(size_t, void *) {
    while(1)
    cout << '-';
}

TaskManager::TaskManager() {}

#pragma pack(push, 1)
struct MZ_Header {
    u16 magic;
    u16 last_page_bytes;
    u16 pages;
    u16 relocation_count;
    u16 header_size;
    u16 min_alloc;
    u16 max_alloc;
    u16 init_ss;
    u16 init_sp;
    u16 checksum;
    u16 init_ip;
    u16 init_cs;
    u16 relocation_table_offset;
    u16 overlay_number;
    // ...
};
#pragma pack(pop)

unsigned TaskManager::exec(String filepath, size_t argc, void *argv) {
    WinHandle win{};
    
    auto executor_base = (MZ_Header *)driverletterspace->mmap(win, filepath, nullptr);
    u32 header_size = executor_base->header_size * 16;
    
    u32 entry_offset = header_size + (executor_base->init_cs * 16) + executor_base->init_ip;
    u8 *enter_point = (u8*)executor_base + entry_offset;
    Multi_Channel_executor.run((void (*)(size_t,void *))enter_point, argc, argv);
    for (unsigned i = 0;i < 512;i++) {
        print_hex(enter_point[i], false);cout << ' ';
    }
    return 0;
}