#include "_idt.hpp"

import lib32;

IDT::IDT(volatile IDT_item *tab):idt_base((IDT::IDT_item *)tab){
    struct {
        unsigned short _1;
        unsigned _2;
    }__attribute__((packed)) idtr;
    idtr._1 = 256 * sizeof(IDT_item) - 1;
    idtr._2 = (unsigned )idt_base;
    asm volatile (
        "lidt %0"
        : 
        : "m"(idtr)
        : "memory"
    );

    for(volatile unsigned i = 0;i < 256 * sizeof(IDT_item);++i){
        reinterpret_cast<volatile unsigned char *>(idt_base)[i] = 0;
    }

    regist(&DE_handler, static_cast<unsigned>(IDNT::_DE)); //除零异常
    regist(&OF_handler, static_cast<unsigned>(IDNT::_OF)); //溢出
    regist(&UD_handler, static_cast<unsigned>(IDNT::_UD)); //无效指令
    regist(&NM_handler, static_cast<unsigned>(IDNT::_NM)); //设备不可用
    regist(&DF_handler, static_cast<unsigned>(IDNT::_DF)); //双重错误
    regist(&GP_handler, static_cast<unsigned>(IDNT::_GP)); //通用保护错误
    regist(&basic_time_handler, 48); //基本时钟中断
};

void IDT::regist(void (*handler)(void), unsigned internum, unsigned char type, unsigned short sec) volatile{
    idt_base[internum].addr_low = (unsigned short)(reinterpret_cast<unsigned>(handler) & 0xFFFF);
    idt_base[internum].selector = sec;
    idt_base[internum].type_attr = type;
    idt_base[internum].addr_high = (unsigned short)((reinterpret_cast<unsigned>(handler) >> 16) & 0xFFFF);
    idt_base[internum].zero = 0;
}

bool IDT::had_handler(unsigned i) const{
    if(idt_base[i].type_attr)return true;
    else return false;
}

unsigned long long _time_count = 0;

extern "C" void DE_handler_c(){
    kprint("#DE: Div error fault!\n");
    while (true){ asm volatile ("hlt"); }
}
extern "C" void OF_handler_c(){
    kprint("#OF: Overflaw fault!\n");
    while (true){ asm volatile ("hlt"); }
}
extern "C" void __attribute__((optimize("O0")))UD_handler_c(_program_status *frame){
    kprint("#UD: Undefined Opcode fault!\n");
    print_program_status(frame);
    catch_program();
}
extern "C" void NM_handler_c(){
    kprint("#NM fault!\n");
    catch_program();
}
extern "C" void DF_handler_c(){
    kprint("#DF: Double fault!\n");
    while (true){ asm volatile ("hlt"); }
}
extern "C" void __attribute__((optimize("O0")))GP_handler_c(_program_status * frame){
    kprint("#GP: General protect fault!\n");
    //print_program_status(frame);
    catch_program();
}
extern "C" void __attribute__((optimize("O0")))basic_time_handler_c(){
    cout << "cut ";
    _time_count++;
};

void print_program_status(_program_status *frame){
    cout << "=== EXCEPTION ===\n" << "EIP: " << frame->eip
    << ", CS: " << frame->cs << '\n';

    cout << "Code at EIP:\n";
    unsigned char *code = (unsigned char *)frame->eip;
    for(int i = 0; i < 16; i++) {
        cout << code[i] << " ";
    }
    cout << '\n';
}