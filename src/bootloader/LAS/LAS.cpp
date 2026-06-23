#include "_LAS.hpp"

LAS* linear_address_space;

LAS::LAS(){
    linear_address_space = this;
}

void LAS::regist(Cluster *clu, String le){
    String letter;
    if (le == String{}) {
        letter = {};
        unsigned count = did_count;
        for (unsigned i = 0;count && i < 11881376 /*  26^5 */;i++) {
            char ch = count % 26;
            letter.append(ch+'A');
            count /= 26;
        }
    }
    else { letter = le; }

    space.append(DriveInfo{
        clu, letter, did_count
    });
    did_count++;
    //kprint("===\nFound a disk! \n    ");kprint(letter);kprint(", Type: ");print_hex((u8)clu->info()->type);kprint(".\n");
}

unsigned LAS::open(_WIN &win, String file_path, u8 mode){
    if(win.extra) { return -1; }
    // win.extra has other handle
    auto path_part = file_path.split(':');
    //for (auto s : path_part) { kprint("token: ");kprint(s);print_char('\n'); }
    for (auto [driver, letter, id] : space) {
        //kprint("Open: ");kprint(letter);print_char('\n');
        if (letter == path_part[0]) {
            auto handle = driver->open(path_part[1]);
            //kprint("Open done: ");print_hex(handle);print_char('\n');
            if (handle == (unsigned)-1) return -1; // not found
            win.extra = new Handle {
                .ID=id, .file_handle=handle, 
                .count=0, .handle_mode=mode
            };
            return handle;
        }
    }
    return -2; // no such disk
}

unsigned LAS::close(_WIN &win){
    if(win.extra){
        Handle* h = static_cast<Handle*>(win.extra);
        auto id = h->ID;
        auto han = h->file_handle;
        delete h;
        win.extra = nullptr;
        return space[id].driver->close(han);
    }
    return -1;
}

unsigned LAS::read(_WIN &win, unsigned byte_offset, unsigned byte_read){
    if(win.extra){
        Handle *h = reinterpret_cast<Handle *>(win.extra);
        for (auto [driver, _, id] : space) {
            if (id == h->ID) {
                auto b = reinterpret_cast<unsigned short *>(&win+1);
                //*
                kprint("read into: ");print_hex((size_t)b);print_char('\n');
                //*/
                return driver->read(
                    b, 
                    h->file_handle, byte_offset, byte_read
                );
            }
        }
    }
    return (unsigned)-1;
}

unsigned LAS::write(_WIN &win, unsigned byte_offset, unsigned byte_write){
    if(win.extra){
        Handle *h = reinterpret_cast<Handle *>(win.extra);
        return space[h->ID].driver->write(
            reinterpret_cast<unsigned short *>(&win+1), 
            h->file_handle, byte_offset, byte_write
        );
    }
    else { return (unsigned)-1; }
}

unsigned LAS::create(_WIN &win, String filename){
    return (unsigned)-1;
}

unsigned LAS::del(_WIN &, String){
    return (unsigned)-1;
}

Cluster_Info LAS::info(_WIN& win, String s){
    if(win.extra) return *space[static_cast<Handle*>(win.extra)->ID].driver->info(s);
    else return {};
}

unsigned LAS::cmd(_WIN &win, unsigned n, String s, void *argv, unsigned argc){
    if(win.extra) return space[static_cast<Handle*>(win.extra)->ID].driver->cmd(n, s,argv, argc);
    else return -1;
}

