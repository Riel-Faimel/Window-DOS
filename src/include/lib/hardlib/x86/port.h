#ifndef __lib_cstdlib_port_h
#define __lib_cstdlib_port_h
#pragma once

extern "C" {
    void __attribute__((regparm(2))) outb(char data, unsigned short port);
    void __attribute__((regparm(2))) outw(unsigned short data, unsigned short port);
    void __attribute__((regparm(2))) outl(unsigned data, unsigned short port);
    unsigned char __attribute__((regparm(1))) inb(unsigned short port);
    unsigned short __attribute__((regparm(1))) inw(unsigned short port);
    unsigned __attribute__((regparm(1))) inl(unsigned short port);
    void io_wait();
    void qps(const char *); //QEMU print string
    inline void at_exit(){};
    inline bool __cxa_guard_acquire(){ return true; }
    inline void __cxa_guard_release(){}
}

#endif