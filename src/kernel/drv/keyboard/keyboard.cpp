#include "_keyboard.hpp"
import changestack;
extern "C" void keyboard_handler();

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

    //outb(0xAA, CMD);
    /*
    io_wait();
    unsigned char self_test = inb(DATA);
    if(self_test != 0x55){
        kout << "[FAILED] Keyboard self check\n";
        return false;
    }
    
    // 键盘接口测试
    outb(0xAB, CMD);
    io_wait();
    unsigned char interface_test = inb(DATA);
    if(interface_test != 0x00){
        kout << "[FAILED] Keyboard interface check\n";
        return false;
    }
    */
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
    DeviceInfo::DeviceType{1}, // ISA_PS2_Config::Device::keyboard
    [] (void *) -> DeviceDriver *{ return new KeyBoard; }
};

KeyBoard *KeyBoard::self = nullptr;

KeyBoard::KeyBoard() {
    self = this;
    click = false;
    buffer.clear();
    interrupt_distributor.reg_irq(&keyboard_handler, IRQ::keyboard);
    if(init())return;
    else kout << "[WARNING] Keyboard Failed\n";
}

void KeyBoard::keyboard_handler_c(){
    static char _stack[1024];
    if (!self) return;
    { ChangeStack s(_stack);{
        unsigned char data = inb(DATA);
        self->buffer.push((char)data);
        self->click = true;
    }}
}

void KeyBoard::work() {
    ;
}

void KeyBoard::off() {
    ;
}