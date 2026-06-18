#include "_LAS.hpp"

LAS* linear_address_space;

//===LAS prepare for Cluster interface===

unsigned Cluster::open(String/*file_path*/) {
    return (unsigned)-1;
}

unsigned Cluster::close(unsigned /*handle*/){
    ;
}

unsigned Cluster::creat(String/*path*/){
    ;
}

unsigned Cluster::delet(String/*path*/){
    ;
}

//===LAS===
LAS::LAS(){
    linear_address_space = this;
}

void LAS::regist(Cluster *clu, String le = {}){
    String letter;
    if (le == String{}) {
        letter = {};
        unsigned count = did_count;
        for (unsigned i = 0;count && i < 26^5;i++) {
            char ch = count % 26;
            letter.append(ch+'A');
            count /= 26;
        }
    }
    else {
        letter = le;
    }

    space.append(DriveInfo{
        clu, letter, did_count
    });
    did_count++;
}

unsigned LAS::read(_WIN &win, unsigned byte_offset, unsigned byte_read){
    if(win.extra){
        Handle *h = reinterpret_cast<Handle *>(win.extra);
        return space[h->ID].driver->read(reinterpret_cast<unsigned short *>(&win+1), h->file_handle, byte_offset, byte_read);
    }
    else { return (unsigned)-1; }
}

unsigned LAS::write(_WIN &win, unsigned byte_offset, unsigned byte_write){
    if(win.extra){
        Handle *h = reinterpret_cast<Handle *>(win.extra);
        return space[h->ID].driver->write(reinterpret_cast<unsigned short *>(&win+1), h->file_handle, byte_offset, byte_write);
    }
    else { return (unsigned)-1; }
}

unsigned LAS::open(_WIN &, String file_path){
    return (unsigned)-1;
}

unsigned LAS::close(_WIN &){
    return (unsigned)-1;
}

unsigned LAS::creat(_WIN &, String){
    return (unsigned)-1;
}

unsigned LAS::del(_WIN &, String){
    return (unsigned)-1;
}

unsigned LAS::info(_WIN&){
    return (unsigned)-1;
}

unsigned LAS::cmd(_WIN &, String){
    return (unsigned)-1;
}

