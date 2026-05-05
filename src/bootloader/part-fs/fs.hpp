#ifndef __bootloader_part_fs_fs_hpp
#define __bootloader_part_fs_fs_hpp
#include <lib/cppstdlib/string>

class FileSystem {
protected:
    String dealing_path;
    friend class LAS;
    inline FileSystem(String init_path):dealing_path{init_path}{}
public:
    virtual unsigned open(String) = 0;
    virtual void close(unsigned) = 0;

    virtual void cd(String) = 0;
    virtual void dir() = 0;
};

#endif