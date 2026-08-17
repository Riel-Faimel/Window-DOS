#include "screen_srv.hpp"

extern "C" { void kprint(char *string){ cout.VGA_screen.print(string); } }
void kprint(const char * str) { kprint(const_cast<char *>(str)); }

void print_hex(unsigned char val, bool a) {
    const char *hex_digits = "0123456789ABCDEF";
    char hex_str[5];
    hex_str[0] = '0';
    hex_str[1] = 'x';
    hex_str[2] = hex_digits[(val >> 4) & 0x0F];
    hex_str[3] = hex_digits[val & 0x0F];
    hex_str[4] = '\0';
    if(a)kprint(hex_str);
    else kprint(hex_str+2);
}

void print_hex(unsigned short val, bool a){
    const char *hex_digits = "0123456789ABCDEF";
    char hex_str[7];
    hex_str[0] = '0';
    hex_str[1] = 'x';
    hex_str[2] = hex_digits[(val >> 12) & 0x0F];
    hex_str[3] = hex_digits[(val >> 8) & 0x0F];
    hex_str[4] = hex_digits[(val >> 4) & 0x0F];
    hex_str[5] = hex_digits[val & 0x0F];
    hex_str[6] = '\0';
    if(a)kprint(hex_str);
    else kprint(hex_str+2);
}

void print_hex(unsigned val, bool a){
    const char *hex_digits = "0123456789ABCDEF";
    char hex_str[11];
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
    hex_str[10]= '\0';
    if(a)kprint(hex_str);
    else kprint(hex_str+2);
}

void print_char(char ch){
    cout.VGA_screen._prt(ch);
}

screen_output cout;