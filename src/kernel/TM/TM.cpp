#include "_TM.hpp"
import lib32;

void caller(size_t n, void *v) {
    void *start_point = *(void **)v;
    void *argv = *((void **)v + 1);
    asm volatile (
        "mov %0, %%eax\n"
        "mov %1, %%edx\n"
        "call %2\n"
        "int $0x31\n" // process exit
        "int $0x32\n" // thread exit
        :
        : "r"(n), "r"(argv), "r"(start_point)
        : "eax", "edx", "memory"
    );
}

TaskManager::MainLoader::MainLoader(): loader_l{} {
    loader_l.append(new exCOM);
}

rtl::pair<TaskManager::MainLoader::ExceptionCode, void *> 
TaskManager::MainLoader::load(const WinHandle &win) {
    for (auto el : loader_l) {
        if(el->execable(win)) {
            el->load();
            return {TaskManager::MainLoader::ExceptionCode::NoError, el->get_entry_point()};
        }
    }
    return {TaskManager::MainLoader::ExceptionCode::UnknowFormat, nullptr};
}

void TaskManager::MainLoader::append(ExecLoader *el) {
    loader_l.append(el);
}


TaskManager::TaskManager() {}

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
    switch (err2) {
    case MainLoader::ExceptionCode::NoError:
        break;
    case MainLoader::ExceptionCode::UnknowFormat:
        return FexecExceptionCode::NoExecutable;
    }
    
    ASMLabelMark(start_thread)
    Multi_Channel_executor.run((void (*)(size_t,void *))entry_point, argc, argv, 1, 3);

    return FexecExceptionCode::NoException;
}