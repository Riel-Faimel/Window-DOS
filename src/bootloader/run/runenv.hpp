#ifndef __bootloader_run_runenv_hpp
#define __bootloader_run_runenv_hpp

class SBEE { //Segment Binary execution environment
public:
    SBEE();//regist program exit interrupt

    void run(void *);//lead and entry 
};

#endif