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
void basic_time_handler(){
    qps("#time\n");
    static unsigned int _time = 0;
    _time++;
    asm volatile ("iret");
}
class IDT {
public:
#pragma pack(push, 1)
struct IDT_item{
    unsigned short addr_low = 0;
    unsigned short selector = 0x08;
    unsigned char zero = 0;
    unsigned char type_attr = 0x8E;
    unsigned short addr_high = 0;
};
#pragma pack(pop)
    volatile IDT_item *idt_base;
    IDT(IDT_item *tab):idt_base(tab){
        struct {
            unsigned short _1;
            unsigned _2;
        }__attribute__((packed)) idtr;
        idtr._1 = 2047;
        idtr._2 = (unsigned )idt_base;
        asm volatile (
            "lidt %0"
            : 
            : "m"(idtr)
            : "memory"
        );
        regist(reinterpret_cast<void *>(&basic_time_handler), 48);
    };

    void regist(void *handler, unsigned internum, unsigned char type = 0x8E, unsigned short sec = 0x08) volatile {
        idt_base[internum].addr_low = (unsigned short)(reinterpret_cast<unsigned>(handler) & 0xFFFF);
        idt_base[internum].selector = 0x08;
        idt_base[internum].type_attr = 0x8E;
        idt_base[internum].addr_high = (unsigned short)((reinterpret_cast<unsigned>(handler) >> 16) & 0xFFFF);
        idt_base[internum].zero = 0;
    }
};

IDT::IDT_item _IDT[256];