#ifndef __kernel_TM_exec_form_exCOM_hpp
#define __kernel_TM_exec_form_exCOM_hpp

#include <global/type.hpp>
#include <interface/exe_resolv.hpp>

class exCOM : public ExecLoader {
    WinHandle win;
    #pragma pack(push, 1)
    struct Header {
        u8 magic[4];
        void *enter_point;
        size_t text_offset;
        void *text_start;
        size_t text_size;
        size_t data_offset;
        void *data_start;
        size_t data_size;
        size_t bss_size;
    } buf;
    #pragma pack(pop)
public:
    exCOM(){}
    bool execable(const WinHandle &win_) override;

    void load() override;

    void *get_entry_point() const override { return buf.enter_point; }
    ~exCOM() override = default;
};


#endif