void outb(char data, unsigned short port){
    asm volatile (
        "outb %0, %1"
        :
        : "a"(data), "Nd"(port)
        : "memory"
    );
}
inline void qps(const char *val){
    for(int i = 0;val[i];i++){
        outb(val[i], 0x3F8);
    }
}
extern "C"[[noreturn]] void basic_time_handler(){
    qps("fuck\n");
    asm volatile (
        ".hang:\n"
        "hlt\n"
        "jmp .hang\n"
    );
}
#pragma pack(push, 1)
struct IDT_item{
    unsigned short addr_low = 0;
    unsigned short selector = 0x08;
    unsigned char zero = 0;
    unsigned char type_attr = 0x8E;
    unsigned short addr_high = 0;
};
#pragma pack(pop)
IDT_item _IDT[256];

extern "C" void LoaderMain(){
    asm volatile ("cli");
    qps("into\n");
LABEL:
    _IDT[48].addr_low = (unsigned short)(reinterpret_cast<unsigned>(&basic_time_handler) & 0xFFFF);
    _IDT[48].selector = 0x08;
    _IDT[48].type_attr = 0x8E;
    _IDT[48].addr_high = (unsigned short)((reinterpret_cast<unsigned>(&basic_time_handler) >> 16) & 0xFFFF);
    _IDT[48].zero = 0;
    struct {
        unsigned short _1;
        unsigned _2;
    }__attribute__((packed)) idtr;
    idtr._1 = 2047;
    idtr._2 = (unsigned )_IDT;
    asm volatile (
        "lidt %0"
        : 
        : "m"(idtr)
        : "memory"
    );
LABEL_END:

    qps("before\n");
    asm volatile ("int $48");
    while(true){asm volatile ("hlt");}
}
/*
extern "C" void LoaderMain(){
    asm volatile ("cli");
    
    // 1. 获取函数地址
    unsigned int handler_addr = (unsigned int)basic_time_handler;
    
    // 2. 设置 IDT 描述符（确保每个字段都正确）
    _IDT[48].addr_low = handler_addr & 0xFFFF;
    _IDT[48].selector = 0x08;           // 代码段选择子
    _IDT[48].zero = 0;                  // 必须为0
    _IDT[48].type_attr = 0x8E;          // 中断门，P=1, DPL=0, 类型=0xE
    _IDT[48].addr_high = (handler_addr >> 16) & 0xFFFF;
    
    // 3. 设置 IDTR（这是最关键的）
    struct {
        unsigned short limit;
        unsigned int base;
    } __attribute__((packed)) idtr;
    
    idtr.limit = 256 * 8 - 1;  // 2047
    idtr.base = (unsigned int)_IDT;
    
    // 4. 加载 IDT（这里必须成功）
    asm volatile ("lidt %0" : : "m"(idtr) : "memory");
    
    // 5. 触发中断
    asm volatile ("int $48");
    
    while(1) asm volatile ("hlt");
}
*/
