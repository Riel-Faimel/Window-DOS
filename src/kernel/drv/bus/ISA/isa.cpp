#include "_isa.hpp"

InsideModuleRecord EnumerMod head {
    []() -> Enumerator * { return new ISA_space{}; }
};

namespace {
constexpr u16 ps2_cmd = 0x64;
constexpr u16 ps2_data = 0x60;
}

bool ISA_space::PS2::wait_input_clear() {
    for (unsigned i = 0; i < 0x10000; ++i) {
        if ((inb(ps2_cmd) & 0x02) == 0) return true;
        io_wait();
    }
    return false;
}

bool ISA_space::PS2::wait_output_full() {
    for (unsigned i = 0; i < 0x10000; ++i) {
        if (inb(ps2_cmd) & 0x01) return true;
        io_wait();
    }
    return false;
}

void ISA_space::PS2::flush() {
    // Bounded: with no controller present inb(0x64) reads 0xFF forever.
    for (unsigned i = 0; i < 0x10000; ++i) {
        if ((inb(ps2_cmd) & 0x01) == 0) return;
        inb(ps2_data);
        io_wait();
    }
}

u8 ISA_space::PS2::read_data() {
    return inb(ps2_data);
}

void ISA_space::PS2::write_cmd(u8 cmd) {
    if (!wait_input_clear()) return;
    outb(cmd, ps2_cmd);
    io_wait();
}

bool ISA_space::PS2::probe() {
    // 先关掉两个端口，清掉上电残留
    write_cmd(0xAD); // disable keyboard
    write_cmd(0xA7); // disable mouse
    flush();

    // 控制器自检
    write_cmd(0xAA);
    if (!wait_output_full()) return false;
    if (read_data() != 0x55) return false;

    // 端口测试：0x00 表示该端口可用
    write_cmd(0xAB); // test keyboard port
    if (wait_output_full())
        has_keyboard = read_data() == 0x00;

    write_cmd(0xA9); // test mouse port
    if (wait_output_full())
        has_mouse = read_data() == 0x00;

    if (has_keyboard) write_cmd(0xAE); // enable keyboard
    if (has_mouse) write_cmd(0xA8);    // enable mouse

    return has_keyboard || has_mouse;
}

void ISA_space::probe() {
    PS2 ps2;
    if (ps2.probe()) {
        pdevspace->append(new DeviceInfo{
            .progif = ProgIFIndex::ISA_PS2_KM,
            .bus = id,
            .cls = DeviceInfo::ClassType::input,
            .device = DeviceInfo::DeviceType{0},
            .config = new ISA_PS2_Config{
                ps2.keyboard(), ps2.mouse(), 0
            },
            .enumer = this
        });
        kout << "[ISA] +device PS/2\n";
    }
}
