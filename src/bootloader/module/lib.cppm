export module lib32;

export extern "C" {
    void __attribute__((regparm(2))) outb(char data, unsigned short port){
        asm volatile (
            "outb %0, %1"
            :
            : "a"(data), "Nd"(port)
            : "memory"
        );
    };
    void __attribute__((regparm(2))) outw(unsigned short data, unsigned short port){
        asm volatile (
            "outw %0, %1"
            :
            : "a"(data), "Nd"(port)
            : "memory"
        );
    };
    void __attribute__((regparm(2))) outl(unsigned data, unsigned short port){
        asm volatile (
            "outl %0, %1"
            :
            : "a"(data), "Nd"(port)
            : "memory"
        );
    };
    unsigned char __attribute__((regparm(1))) inb(unsigned short port){
        unsigned char data;
        asm volatile (
            "inb %1, %0"
            : "=a"(data)
            : "Nd"(port)
            : "memory"
        );
        return data;
    };
    unsigned short __attribute__((regparm(1))) inw(unsigned short port){
        unsigned short data;
        asm volatile (
            "inw %1, %0"
            : "=a"(data)
            : "Nd"(port)
            : "memory"
        );
        return data;
    };
    unsigned __attribute__((regparm(1))) inl(unsigned short port){
        unsigned data;
        asm volatile (
            "inl %1, %0"
            : "=a"(data)
            : "Nd"(port)
            : "memory"
        );
        return data;
    };
    void io_wait(){
        outb(0, 0x80);
    };
    void qps(const char *val){
        for(int i = 0;val[i];i++){
            outb(val[i], 0x3F8);
        }
    }; //QEMU print string
}