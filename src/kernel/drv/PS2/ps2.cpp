#include "_ps2.hpp"
import changestack;
extern "C" void ps2_handler();

bool init(){
    unsigned time_count = 0;
    outb(0xAD, CMD);
    io_wait();
    
    outb(0xA7, CMD);
    io_wait();
    
    // 清空输出缓冲区
    while (inb(CMD) & 0x01) {
        inb(DATA);
        io_wait();
        time_count++;
        if(time_count > 0x100){
            kout << "time out\n";
            return false;
        }
    }
    
    // 读取配置字节
    outb(0x20, CMD);
    io_wait();
    unsigned char config = inb(DATA);
    config |= 0x01;
    config &= ~0x10; // 禁用鼠标中断
    outb(0x60, CMD);
    io_wait();
    outb(config, DATA);
    io_wait();
    outb(0xAE, CMD);
    io_wait();

    while (inb(CMD) & 0x01) {
        inb(DATA);
        io_wait();
        time_count++;
        if(time_count > 0x100){
            kout << "time out\n";
            return false;
        }
    }
    return true;
}

InsideModuleRecord PDevDrv head {
    ProgIFIndex::ISA_PS2_KM,
    DeviceInfo::ClassType::input,
    [] (void *) -> DeviceDriver *{ return new ps2; }
};

ps2 *ps2::self = nullptr;

ps2::ps2() {
    self = this;
    interrupt_distributor.reg_irq(&ps2_handler, IRQ::keyboard);
    if(init())return;
    else kout << "[WARNING] ps2 Failed\n";
}

void ps2::ps2_handler_c(){
    static char _stack[1024];
    {ChangeStack s(_stack);{
        u8 status = inb(CMD);
        u8 data = inb(DATA);
        if (status & 0x20)
            self->mouse_buffer.push(data);
        else
            self->buffer.push(data);
    }}
}

void ps2::work() {
    kom->append(new PS2_KB{*this});
    kom->append(new PS2_MS{*this});
}

void ps2::off() {
    // delete obj
    buffer.clear();
    mouse_buffer.clear();
}

unsigned PS2_KB::take(void *buf, unsigned, unsigned num) {
    if (!buf || num == 0) return 0;

    auto out = static_cast<char *>(buf);
    unsigned count = 0;

    while (count < num && !drv.buffer.empty()) {
        u8 data = drv.buffer.front();

        if (skip) {
            drv.buffer.pop(data);
            skip--;
            continue;
        }
        if (data == 0xE1) {
            drv.buffer.pop(data);
            skip = 5;
            extended = false;
            continue;
        }
        if (data == 0xE0) {
            drv.buffer.pop(data);
            extended = true;
            continue;
        }

        bool released = data & 0x80;
        u8 code = data & 0x7F;

        if (extended) {
            switch (code) {
            case 0x1D:
                drv.buffer.pop(data);
                extended = false;
                ctrl = !released;
                continue;
            case 0x38:
                drv.buffer.pop(data);
                extended = false;
                alt = !released;
                continue;
            default:
                break;
            }
            if (released) {
                drv.buffer.pop(data);
                extended = false;
                continue;
            }

            const ps2_seq &seq = ps2_ext_seq[code];
            if (!seq.data || seq.len == 0) {
                drv.buffer.pop(data);
                extended = false;
                continue;
            }
            if (count + seq.len > num)
                break;

            drv.buffer.pop(data);
            extended = false;
            for (unsigned i = 0; i < seq.len; i++)
                out[count++] = seq.data[i];
            continue;
        }

        switch (code) {
        case 0x2A:
            drv.buffer.pop(data);
            left_shift = !released;
            continue;
        case 0x36:
            drv.buffer.pop(data);
            right_shift = !released;
            continue;
        case 0x1D:
            drv.buffer.pop(data);
            ctrl = !released;
            continue;
        case 0x38:
            drv.buffer.pop(data);
            alt = !released;
            continue;
        case 0x3A:
            drv.buffer.pop(data);
            if (!released) caps = !caps;
            continue;
        default:
            break;
        }

        if (released) {
            drv.buffer.pop(data);
            continue;
        }

        bool shift = left_shift || right_shift;
        char ch = shift ? shift_code2ascii[code] : 0;
        if (!ch) ch = code2ascii[code];
        if (!ch) {
            drv.buffer.pop(data);
            continue;
        }

        if (caps) {
            if (ch >= 'a' && ch <= 'z') ch -= 'a' - 'A';
            else if (ch >= 'A' && ch <= 'Z') ch += 'a' - 'A';
        }
        if (ctrl) {
            if (ch >= 'a' && ch <= 'z') ch = ch - 'a' + 1;
            else if (ch >= 'A' && ch <= 'Z') ch = ch - 'A' + 1;
        }
        if (alt) {
            drv.buffer.pop(data);
            continue;
        }

        if (count + 1 > num)
            break;

        drv.buffer.pop(data);
        out[count++] = ch;
    }
    return count;
}