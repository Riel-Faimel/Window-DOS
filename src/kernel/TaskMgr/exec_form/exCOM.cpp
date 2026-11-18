#include "exCOM.hpp"
#include <SrvDevMgr/DLS.hpp>
#define LocalDebug
#include <global/macro.hpp>

bool exCOM::execable(const WinHandle &win_) {
    win = win_;

    driverletterspace->read(&buf, (DLS::Handle *)win.extra, 0, sizeof(Header));
    if (buf.magic[0] != 'E' || buf.magic[1] != 'C' || 
        buf.magic[2] != 'O' || buf.magic[3] != 'M'
    ) {
        LocalDebugInfo(kout
            << "[TM] <exception> Unknow Head: " 
            << (char)buf.magic[0] << (char)buf.magic[1] 
            << (char)buf.magic[2] << (char)buf.magic[3] 
        << '\n';)
        return false;
    } else { return true; }
}

void exCOM::load() {
    auto h = (DLS::Handle *)win.extra;
    driverletterspace->read((void*)buf.text_start, h, buf.text_offset, buf.text_size);
    if (buf.data_size > 0) {
        driverletterspace->read((void*)buf.data_start, h, buf.data_offset, buf.data_size);
    }
    if (buf.bss_size > 0) {
        __builtin_memset((void*)((u8 *)buf.data_start + buf.data_size), 0, buf.bss_size);
    }

    #undef LocalDebug
    #include <global/macro.hpp>
    LocalDebugInfo(
    kout    << "[exCOM] <Info> :" 
            << "entry=" << buf.enter_point << '\n' 
            << "text off=" << buf.text_offset << '\n' 
            << "test addr=" << buf.text_start << '\n' 
            << "test size=" << buf.text_size << '\n' 
            << "data off=" << buf.data_offset << '\n' 
            << "data addr=" << buf.data_start << '\n' 
            << "data size=" << buf.data_size << "\n====\n";
    )
}