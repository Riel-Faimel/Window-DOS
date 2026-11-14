#ifndef __kernel_TM_exec_form_MZ_hpp
#define __kernel_TM_exec_form_MZ_hpp

#include <global/type.hpp>

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


#endif