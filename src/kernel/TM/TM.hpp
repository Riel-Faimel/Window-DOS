#ifndef __kernel_TM_TM_hpp
#define __kernel_TM_TM_hpp

/**
 * TaskManager is global task start entry
 * task include thread and sources list
 */

#include <lib/string>
#include <HL/container>
#include <multiCE/Multi-Channel_exec.hpp>
#include <interface/exe_resolv.hpp>
#include <interface/TD/PCB.hpp>

enum class FexecExceptionCode : u8 {
    // done
    NoException,

    // exception
    OpenFileFailed,
    NoExecutable,

    // fault
    DLSFault,
};

class TaskManager {
    class MainLoader {
        rtl::array<ExecLoader *> loader_l;
    public:
        enum class ExceptionCode {
            NoError,

            UnknowFormat,
        };
        MainLoader();
        rtl::pair<ExceptionCode, void *> load(const WinHandle &);
        void append(ExecLoader *);
    } mainloader;

    rtl::list<PCB> pcb_l;
public:
    TaskManager();

    FexecExceptionCode exec(String, size_t, void *);
    void exit(u32 id, u32 code);
};

#endif