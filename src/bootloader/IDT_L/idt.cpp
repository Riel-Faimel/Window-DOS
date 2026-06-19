#include "_idt.hpp"

import lib32;

void qpl_hex(unsigned val){
    const char *hex_digits = "0123456789ABCDEF";
    char hex_str[12];
    hex_str[0] = '0';
    hex_str[1] = 'x';
    hex_str[2] = hex_digits[(val >> 28) & 0x0F];
    hex_str[3] = hex_digits[(val >> 24) & 0x0F];
    hex_str[4] = hex_digits[(val >> 20) & 0x0F];
    hex_str[5] = hex_digits[(val >> 16) & 0x0F];
    hex_str[6] = hex_digits[(val >> 12) & 0x0F];
    hex_str[7] = hex_digits[(val >> 8) & 0x0F];
    hex_str[8] = hex_digits[(val >> 4) & 0x0F];
    hex_str[9] = hex_digits[val & 0x0F];
    hex_str[10]= '\n';
    hex_str[11]= '\0';
    qps(hex_str);
}

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
    regist(&basic_time_handler, static_cast<unsigned>(IDNT::time)); //基本时钟中断
};

void IDT::regist(void (*handler)(void), unsigned int internum, unsigned char type, unsigned short sec) volatile{
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

__attribute__((naked)) void IDT::set_PIC() volatile {
    asm volatile (
        "movb $0x11, %%al\n"
        "outb %%al, %0\n"
        "movb $0x11, %%al\n"
        "outb %%al, %1\n"
        "call io_wait\n"
        
        "movb $0x30, %%al\n"
        "outb %%al, %2\n"
        "movb $0x38, %%al\n"
        "outb %%al, %3\n"
        "call io_wait\n"
        
        "movb $0x04, %%al\n"
        "outb %%al, %2\n"
        "movb $0x02, %%al\n"
        "outb %%al, %3\n"
        "call io_wait\n"
        
        "movb $0x01, %%al\n"
        "outb %%al, %2\n"
        "movb $0x01, %%al\n"
        "outb %%al, %3\n"
        "call io_wait\n"

        "ret\n"
        :
        : "N" (PIC1_CMD), "N" (PIC2_CMD),
          "N" (PIC1_DATA), "N" (PIC2_DATA)
        : "al", "memory"
    );
}

unsigned long long _time_count = 0;

extern "C" void DE_handler_c(){
    screen->print("#DE: Div error fault!\n");
    qps("#UD: Unknown fault!\n");
    while (true){
        asm volatile ("hlt");
    };
}
extern "C" void OF_handler_c(){
    screen->print("#OF: Overflaw fault!\n");
    qps("#OF: Overflaw fault!\n");
    while (true){
        asm volatile ("hlt");
    };
}
extern "C" void __attribute__((optimize("O0")))UD_handler_c(_program_status *frame){
    screen->print("\n#UD: Undefined Opcode fault!\n");
    qps("#UD: Undefined Opcode fault!\n");
    print_program_status(frame);
    catch_program();
}
extern "C" void NM_handler_c(){
    screen->print("\r\n#NM fault!\r\n");
    qps("#NM fault!\r\n");
    catch_program();
}
extern "C" void DF_handler_c(){
    screen->print("\r\n#DF: Double fault!\r\n");
    qps("#DF: Double fault!\r\n");
    while (true){
        asm volatile ("hlt");
    };
}
extern "C" void __attribute__((optimize("O0")))GP_handler_c(_program_status * /*frame*/){
    screen->print("\r\n#GP: General protect fault!\r\n");
    qps("#GP: General protect fault!\n");
    //print_program_status(frame);
    catch_program();
}
extern "C" void __attribute__((optimize("O0")))basic_time_handler_c(){
    //qps("time cut\n");
    _time_count++;
};

void print_program_status(_program_status *frame){
    screen->print("\r\n=== EXCEPTION ===\r\n");
    
    // 1. 异常发生位置
    screen->print("EIP: "); print_hex(frame->eip);
    screen->print("  CS: "); print_hex(frame->cs);
    screen->print("\r\n");

    // 4. 附近指令 (最关键的！)
    screen->print("Code at EIP:\r\n");
    unsigned char *code = (unsigned char *)frame->eip;
    for(int i = 0; i < 16; i++) {
        print_hex(code[i]);
        screen->print(" ");
    }
    screen->print("\n");
    
    // 5. 栈回溯 (可选)
    /*
    
    // 2. 栈状态
    screen->print("ESP: "); print_hex(frame->esp);
    screen->print("  SS: "); print_hex(frame->ss);
    screen->print("\n");
    screen->print("Stack trace:\n");
    unsigned *stack = (unsigned *)frame->esp;
    for(int i = 0; i < 8; i++) {
        print_hex(stack[i]);
        screen->print(" ");
    }
    
    // 7. 段寄存器
    unsigned short ds, es;
    asm volatile("mov %%ds, %0" : "=r"(ds));
    asm volatile("mov %%es, %0" : "=r"(es));
    
    screen->print("DS: "); print_hex(ds);
    screen->print(" ES: "); print_hex(es);
    screen->print("\n");
    screen->print("\n");*/
}

void __attribute__((optimize("O0")))IDT::idtt() volatile {
    screen->print("idt object  idt table address :\r\n");
    print_hex((unsigned)this);
    screen->print("\r\n");
    print_hex((unsigned )idt_base);
    screen->print("\r\n");
}