#include "_idt.hpp"

import lib32;

IDT::IDT(volatile IDT_item *tab):idt_base(tab){
    static_assert(sizeof(IDT_item) == 8, "error");
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

    regist(&DE_handler, static_cast<unsigned>(IDNT::_DE)); //除零异常
    regist(&OF_handler, static_cast<unsigned>(IDNT::_OF)); //溢出
    regist(&UD_handler, static_cast<unsigned>(IDNT::_UD)); //无效指令
    regist(&NM_handler, static_cast<unsigned>(IDNT::_NM)); //设备不可用
    regist(&DF_handler, static_cast<unsigned>(IDNT::_DF), 0x80, 0); //双重错误
    regist(&GP_handler, static_cast<unsigned>(IDNT::_GP)); //通用保护错误
    regist(&basic_time_handler, 48); //基本时钟中断

#ifdef _DEBUG
    cout << ", IDT: " << (void *)tab;
#endif
};

void IDT::regist(
    void (*handler)(void), unsigned internum, unsigned short sec, 
    unsigned char DPL, bool _32_or_16
) volatile {
    new ((void *)&idt_base[internum]) descrptor::IDTEntry::IG {
        .addr_low = (unsigned short)(reinterpret_cast<unsigned>(handler) & 0xFFFF),
        .selector = sec, .is_32_bits_or_16_bits = (unsigned char)(_32_or_16?1:0),
        .zero = 0, .DPL = DPL, 
        .addr_high = (unsigned short)((reinterpret_cast<unsigned>(handler) >> 16) & 0xFFFF),
    };
}

bool IDT::had_handler(unsigned i) const{
    if(idt_base[i].Int_Gate.exist)return true;
    else return false;
}

unsigned long long _time_count = 0;

extern "C" {

void DE_handler_c(){
    kprint("#DE: /0!\n");
    while (true){ asm volatile ("hlt"); }
}
void OF_handler_c(){
    kprint("#OF: Overflaw!\n");
    while (true){ asm volatile ("hlt"); }
}
__attribute__((optimize("O0")))
void UD_handler_c(no_errcode_frame *frame){
    kprint("=== #UD ===\n");
    no_errcode_frame_status(frame);
    catch_program();
}
void NM_handler_c(){
    kprint("#NM fault!\n");
    catch_program();
}
void DF_handler_c(){
    kprint("#DF: Double fault!\n");
    while (true){ asm volatile ("hlt"); }
}
__attribute__((optimize("O0")))
void GP_handler_c(errcode_frame * frame){
    cout << "=== #GP ===\n";
    errorcode_frame_status(frame);
    catch_program();
}
__attribute__((optimize("O0")))
void basic_time_handler_c(){
    cout << "cut ";
    _time_count++;
};

}

void no_errcode_frame_status(no_errcode_frame *frame) {
    cout << "\n< CS = " << frame->cs << ", EIP = " << frame->eip << " >\n";
}

void errorcode_frame_status (errcode_frame *frame) {
    cout << "Error Code: " << frame->errcode;
    cout << "\n< CS = " << frame->cs << ", EIP = " << frame->eip << " >\n";
    if (frame->errcode & 0b0100 == 0)return;
    cout << "< SS = " << frame->ss << ", ESP = " << frame->esp << " >\n";
}

