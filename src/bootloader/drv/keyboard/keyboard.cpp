#include "_keyboard.hpp"
import changestack;
char _stack[128];
extern "C" void keyboard_handler();

namespace KeyBoard
{
char buffer[1024];
bool click;
unsigned read;
unsigned write;

inline void deal_main(){
    unsigned char data = inb(DATA);
    buffer[write] = data;
    write = (write + 1) & 1023;
    click = true;
    unsigned esp;
}

void keyboard_handler_c(){
    {
        ChangeStack s(_stack);
        {
            deal_main();
        }
    }
    /*
    mov al, 0x20
    out 0x20, al
    */
}

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
            screen->print("time out\n");
            return false;
        }
    }
    
    // 读取配置字节
    outb(0x20, CMD);
    io_wait();
    unsigned char config = inb(DATA);
    config |= 0x01;
    config &= ~0x10; // 禁用鼠标中断，如果需要
    outb(0x60, CMD);
    io_wait();
    outb(config, DATA);
    io_wait();

    //outb(0xAA, CMD);
    /*
    io_wait();
    unsigned char self_test = inb(DATA);
    if(self_test != 0x55){
        screen->print("[FAILED] Keyboard self check\n");
        return false;
    }
    
    // 键盘接口测试
    outb(0xAB, CMD);
    io_wait();
    unsigned char interface_test = inb(DATA);
    if(interface_test != 0x00){
        screen->print("[FAILED] Keyboard interface check\n");
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
            screen->print("time out\n");
            return false;
        }
    }
    return true;
}

void init_driver(){
    click = false;
    read = 0;
    write = 0;
    idt.regist(&keyboard_handler, static_cast<unsigned>(IDNT::keyboard));
    if(init())return;
    else{
        screen->print("[WARNING] Keyboard Failed\n");
    };
}

} // namespace KeyBoard
