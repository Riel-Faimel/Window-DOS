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

TaskManager::TaskManager() {}

#pragma pack(push, 1)
struct exCOM_Header {
    u8 magic[4];
    void *enter_point;
    size_t text_offset;
    void *text_start;
    size_t text_size;
    size_t data_offset;
    void *data_start;
    size_t data_size;
    size_t bss_size;
};
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

void *exCOM_resolv(WinHandle &win) {
    auto h = (DLS::Handle *)win.extra;
    exCOM_Header buf;
    driverletterspace->read(&buf, h, 0, sizeof(buf));

    LocalDebugInfo(cout << buf.enter_point << ' ' << buf.text_offset << ' ' << buf.text_start << ' ' << buf.text_size << ' '\
    << buf.data_offset << ' ' << buf.data_start << ' ' << buf.data_size << ' ';)
    
    if (buf.magic[0] != 'E' || buf.magic[1] != 'C' || 
        buf.magic[2] != 'O' || buf.magic[3] != 'M') {
        return nullptr;
    }
    
    driverletterspace->read((void*)buf.text_start, h, buf.text_offset, buf.text_size);
    if (buf.data_size > 0) {
        driverletterspace->read((void*)buf.data_start, h, buf.data_offset, buf.data_size);
    }
    if (buf.bss_size > 0) {
        __builtin_memset((void*)(buf.data_start + buf.data_size), 0, buf.bss_size);
    }
    return buf.enter_point;
}

#undef LocalDebug
#include <global/macro.hpp>
unsigned TaskManager::exec(String filepath, size_t argc, void *argv) {
    WinHandle win{};
    
    auto err = driverletterspace->open(win, filepath);
    if (err) return err;
    u8 *enter_point = (u8 *)exCOM_resolv(win);
    
    Multi_Channel_executor.run((void (*)(size_t,void *))enter_point, argc, argv, 1, 3);

    LocalDebugInfo(for (unsigned i = 0;i < 256;i++) { print_hex(enter_point[i], false);cout << ' '; })
    return 0;
}