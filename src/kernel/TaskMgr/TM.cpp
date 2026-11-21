#include "_TM.hpp"
#include <lib32.hpp>

TaskManager *taskmgr;

void caller(size_t n, void *v) {
    void *start_point = *(void **)v;
    void *argv = *((void **)v + 1);
    asm volatile (
        "mov %0, %%eax\n"
        "mov %1, %%edx\n"
        "call *%2\n"
        "int $0x31\n" // process exit
        "int $0x32\n" // thread exit
        :
        : "r"(n), "r"(argv), "r"(start_point)
        : "eax", "edx", "memory"
    );
}

// 构造函数里不建任何 loader：像 exCOM 这样的格式对象持有 WinHandle，
// 而 WinHandle 要求 WindowManager 已经在场。TaskManager 比它先构造，
// 所以注册推迟到第一次 load()——那时子系统都已经建好。
TaskManager::MainLoader::MainLoader(): loader_l{} {}

rtl::pair<TaskManager::MainLoader::ExceptionCode, void *> 
TaskManager::MainLoader::load(const WinHandle &win) {
    if (loader_l.get_size() == 0) loader_l.append(new exCOM);

    for (auto el : loader_l) {
        if(el->execable(win)) {
            el->load();
            return {TaskManager::MainLoader::ExceptionCode::NoError, el->get_entry_point()};
        }
    }
    return {TaskManager::MainLoader::ExceptionCode::UnknowFormat, nullptr};
}

void TaskManager::MainLoader::append(ExecLoader *el) { loader_l.append(el); }


TaskManager::TaskManager():mainloader{}, pcb_l{} { taskmgr = this; }

FexecExceptionCode TaskManager::exec(String filepath, size_t argc, void *argv) {
    WinHandle win{};
    
    auto err1 = driverletterspace->open(win, filepath);
    if (err1) switch (err1) {
    case ClusterDeviceManagerDefination_OpenErrorCode_NoSuchDisk:
    case ClusterDeviceManagerDefination_OpenErrorCode_NotCloseHandler: {
        return FexecExceptionCode::OpenFileFailed;
    } break;
    default: {
        return FexecExceptionCode::DLSFault;
    } break;
    }
    auto [err2, entry_point] = mainloader.load(win);
    if (err2 != MainLoader::ExceptionCode::NoError) {
        driverletterspace->close(win);
        return FexecExceptionCode::NoExecutable;
    }

    // The image is in memory now; the file handle is no longer needed.
    driverletterspace->close(win);
    
    // TODO: set MemSpace
    Multi_Channel_executor.run((void (*)(size_t,void *))entry_point, argc, argv, {}, 1, 3);

    return FexecExceptionCode::NoException;
}