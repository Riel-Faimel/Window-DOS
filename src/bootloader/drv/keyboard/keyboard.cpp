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
}

void keyboard_handler_c(){
    {
        ChangeStack s(_stack);
        {
            deal_main();
        }
    }
}

bool init(){
    outb(0xAD, CMD);
    io_wait();

    outb(0x20, CMD);
    io_wait();
    unsigned char config = inb(DATA);
    config |= 0x01;
    outb(0x60, CMD);
    io_wait();
    outb(config, DATA);
    io_wait();

    outb(0xAA, CMD);
    io_wait();
    if(inb(DATA) != 0x55) return false;
    outb(0xAE, CMD);
    io_wait();

    while (inb(CMD) & 0x01) {
        inb(DATA);
        io_wait();
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
