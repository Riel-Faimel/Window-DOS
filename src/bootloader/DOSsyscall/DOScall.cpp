#include "DOScall.hpp"
#include <drv/screen/screen_srv.hpp>

extern "C" {
    void program_exit(){};
    /**
     * cs = PSP section address
     */
    void standard_input_with_echo(){};
    /**
     * read byte from keyboard, echo
     */
    void standard_output(char dl){
        print_char(dl);
    };
    /**
     * print byte to console
     * dl = byte to print
     */
    void direct_console_input_no_echo(){};
    void direct_console_input_with_ctrl_c(){};
    void print_string(char *edx){
        for(unsigned i = 0;i < 0x1000;i++){
            if(edx[i] == '$'){
                edx[i] = '\0';
                screen->print(edx);
                return;
            }
        }
        //error
        return;
    };
    void buffered_keyboard_input(){};
    void check_stdin_status(){};
    void flush_buffer_and_input(){};
    void create_file(){};
    void open_file(){};
    void close_file(){};
    void read_file(){};
    void write_file(){};
    void seek_file(){};
    void allocate_memory(){};
    void free_memory(){};
    void modify_memory_block(){};
    void exit_with_code(){};
    void terminate_and_stay_resident(){};
    void change_directory(){};
    void get_current_directory(){};
    void get_date(){};
    void get_time(){};
    void set_interrupt_vector(){};
    void get_interrupt_vector(){};
    void get_current_drive(){};
}

extern "C" void DOScall_handler();
extern "C" void DOS_exit(){};

extern "C" void DOScall_handler_c(
    unsigned syscall_number,
    unsigned /*ebx_val*/,
    unsigned /*ecx_val*/,
    unsigned edx_val,
    unsigned /*esi_val*/,
    unsigned /*edi_val*/
) {
    /*
    asm volatile (
        ".globl __LABEL\n"
        "__LABEL:\n"
    );
    screen->print("call ");print_hex(syscall_number);
    screen->print(" :\n");
    print_hex(ebx_val);
    screen->print("\n");
    print_hex(ecx_val);
    screen->print("\n");
    print_hex(edx_val);
    screen->print("\n");
    print_hex(esi_val);
    screen->print("\n");
    print_hex(edi_val);
    screen->print("\n");
    */
    switch(syscall_number) {
        case 0x00:  // 程序终止
            program_exit();
            break;
        case 0x01:  // 带回显键盘输入
            standard_input_with_echo();
            break;
        case 0x02:  // 字符输出
            standard_output(edx_val);
            break;
        case 0x07:  // 无回显键盘输入（不处理Ctrl+C）
            direct_console_input_no_echo();
            break;
        case 0x08:  // 无回显键盘输入（处理Ctrl+C）
            direct_console_input_with_ctrl_c();
            break;
        case 0x09:  // 字符串输出（$结尾）
            print_string(reinterpret_cast<char *>(edx_val));
            break;
        case 0x0A:  // 缓冲字符串输入
            buffered_keyboard_input();
            break;
        case 0x0B:  // 检查键盘状态
            check_stdin_status();
            break;
        case 0x0C:  // 清空键盘缓冲区并调用指定输入功能
            flush_buffer_and_input();
            break;
        
        // 文件操作组 (0x3C-0x42)
        case 0x3C:  // 创建文件
            create_file();
            break;
        case 0x3D:  // 打开文件
            open_file();
            break;
        case 0x3E:  // 关闭文件
            close_file();
            break;
        case 0x3F:  // 读取文件
            read_file();
            break;
        case 0x40:  // 写入文件
            write_file();
            break;
        case 0x42:  // 移动文件指针
            seek_file();
            break;
        
        // 内存管理组 (0x48-0x4A)
        case 0x48:  // 分配内存
            allocate_memory();
            break;
        case 0x49:  // 释放内存
            free_memory();
            break;
        case 0x4A:  // 修改内存块大小
            modify_memory_block();
            break;
        
        // 进程退出组
        case 0x4C:  // 带返回码退出
            exit_with_code();
            break;
        case 0x31:  // 终止并驻留
            terminate_and_stay_resident();
            break;
        
        // 目录操作组
        case 0x3B:  // 改变当前目录
            change_directory();
            break;
        case 0x47:  // 获取当前目录
            get_current_directory();
            break;
        
        // 时间日期组
        case 0x2A:  // 获取日期
            get_date();
            break;
        case 0x2C:  // 获取时间
            get_time();
            break;
        
        // 中断向量组
        case 0x25:  // 设置中断向量
            set_interrupt_vector();
            break;
        case 0x35:  // 获取中断向量
            get_interrupt_vector();
            break;
        
        // 其他
        case 0x19:  // 获取当前盘符
            get_current_drive();
            break;
        
        default:
            // 未实现的功能号，设置进位标志表示错误
            // set_carry_flag();
            break;
    }
}

DOScall::DOScall(IDT& idt) {
    idt.regist(DOScall_handler, 0x21);
    idt.regist(DOS_exit, 0x20);
}