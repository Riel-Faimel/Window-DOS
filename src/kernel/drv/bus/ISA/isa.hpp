#ifndef __kernel_PDS_ISA_isa_hpp
#define __kernel_PDS_ISA_isa_hpp

#include <interface/hardev/enumer.hpp>

struct ISA_PS2_Config {
    bool has_keyboard;
    bool has_mouse;
    u8 controller_config;
};

class ISA_space : public Enumerator {
    class PS2 {
    public:
        bool probe();
        bool keyboard() const { return has_keyboard; }
        bool mouse() const { return has_mouse; }

    private:
        bool has_keyboard = false;
        bool has_mouse = false;

        static bool wait_input_clear();
        static bool wait_output_full();
        static void flush();
        static u8 read_data();
        static void write_cmd(u8 cmd);
    };

public:
    void probe() override;
    // ISA 没有配置空间，设备门控靠固定端口，忽略配置请求。
    void set_mod(void *) override {}
};


#endif
