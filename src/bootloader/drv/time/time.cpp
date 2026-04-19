#include "_time.hpp"

extern "C" void time_handler();
extern "C" inline void Time::inc(){
    _time_count++;
}
//======
//extern unsigned o_Hz;
//extern unsigned p_Hz;
//======

volatile Time *Time::self;
Time::Time(/* args */):
Hz(18){
    self = this;
    if(!idt.had_handler(32)){
        IDT::IDT_item item;
        item.addr_low = reinterpret_cast<unsigned>(&time_handler) & 0xFFFF;
        item.addr_high = (reinterpret_cast<unsigned>(&time_handler) >> 16) & 0xFFFF;
        idt.regist(&time_handler, static_cast<unsigned>(IDNT::time));
    }
    //set_Hz(1000);
}

Time::~Time(){}
extern "C" void time_handler_c(){
    Time::inc();
}

constexpr unsigned BASE_FREQ = 1193180;
void Time::set_Hz(unsigned int hz){
    unsigned d = BASE_FREQ / hz;

    outb(0x43, 0x36);
    io_wait();
    outb(0x40, d & 0xFF);
    io_wait();
    outb(0x40, (d >> 8) & 0xFF);

    Hz = hz;
}

extern "C" void _sleep(unsigned time){
    unsigned long long count = _time_count;
    while(_time_count - count < time * 18 / 1000){
        asm volatile ("hlt");
    }
    print_hex((unsigned)_time_count);
    screen->print("^^^^^^^^");
};