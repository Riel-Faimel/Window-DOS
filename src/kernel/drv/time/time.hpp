#ifndef __kernel_drv_time_hpp
#define __kernel_drv_time_hpp
#pragma once

class Time{
public:
    volatile static Time *self;
    unsigned Hz;

    //friend void _sleep(unsigned time);
    Time();
    ~Time();

    static inline void inc();
    void set_Hz(unsigned hz);
};

extern "C" void _sleep(unsigned time);

#endif