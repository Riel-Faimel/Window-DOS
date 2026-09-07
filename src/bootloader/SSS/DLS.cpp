#include "_DLS.hpp"

DLS* driverletterspace;

DLS::DLS(){ driverletterspace = this; }

void DLS::regist(Cluster *clu, String le){
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

    space.append(DriveInfo{clu, did_count, letter});
    did_count++;
    cout << "[INFO] Get one FS [" << letter << ":], ID = " << (u8)clu->info(0)->type << '\n';
}

unsigned DLS::start(WinHandle &win, String driver_letter) {
    auto len = driver_letter.length();
    auto path = driver_letter.get_word(':');
    auto hptr = (Handle *)win.handle;
    if (path.length() >= len) {
        // not found ':'
        return -1;
    }
    if (hptr) {
        // need to close
        Cluster *disk = nullptr;
        unsigned new_id = 0;
        for (auto [driver, id, letter] : space) {
            if (hptr->ID == id) {
                disk = driver;
            }
            if (letter == path) {
                new_id = id;
            }
        }
        if (new_id == hptr->ID) {
            return 0;
        } else {
            delete hptr;
            hptr = new Handle { .ID{new_id}, .file_handle{0} };
            return 0;
        }
    } else {
        for (auto [driver, id, letter] : space)
        if (letter == path) {
            win.extra = new Handle { .ID{id}, .file_handle{0}, };
            return 0;
        }
    }
    return -1;
}

unsigned DLS::open(WinHandle &win, String file_path){
    if(win.extra) { return -1; }
    // win.extra has other handle
    auto path_part = file_path.split(':');
    for (auto [driver, id, letter] : space) {
        if (letter == path_part[0]) {
            auto handle = driver->open(path_part[1]);
            if (handle == (unsigned)-1) return -1; // not found
            win.extra = new Handle { .ID=id, .file_handle=handle, };
            return 0;
        }
    }
    return -2; // no such disk
}

unsigned DLS::close(WinHandle &win){
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

unsigned DLS::read(void *buf, Handle *h, unsigned byte_offset, unsigned byte_read){
    if (auto d = search_disk(h);d)
    return d->read(buf, h->file_handle, byte_offset, byte_read);
    else return -1;
}

unsigned DLS::write(void *buf, Handle *h, unsigned byte_offset, unsigned byte_write){
    if (auto d = search_disk(h);d)
    return d->write(buf, h->file_handle, byte_offset, byte_write);
    else return -1;
}

unsigned DLS::create(WinHandle &win, String filename){
    return (unsigned)-1;
}

unsigned DLS::del(WinHandle &, String){
    return (unsigned)-1;
}

ClusterInfo *DLS::info(Handle *h, String s){
    if (auto d = search_disk(h);d)
    return d->info(h->file_handle, s);
    else return nullptr;
}

unsigned DLS::cmd(Handle *h, unsigned n, String s, void *argv, unsigned argc){
    if (auto d = search_disk(h);d)
    return d->cmd(n, s, argv, argc);
    else return -1;
}

void *DLS::mmap(Handle *h, void *nice_address = nullptr) {
    if (auto d = search_disk(h);d)
    return d->mmap(h->file_handle, nice_address);
    else return nullptr;
}

Cluster *DLS::search_disk(Handle *h) {
    if (h)
    for (auto [driver, id, letter] : space)
    if (h->ID == id)
    return driver;
    return nullptr;
}
