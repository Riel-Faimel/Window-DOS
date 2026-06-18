#ifndef __bootloader_part_fs_fs_hpp
#define __bootloader_part_fs_fs_hpp
#include <lib/cppstdlib/string>

struct FileInfo {};

class FileSystem {
protected:
    friend class LAS;
    FileSystem() = default;
public:
    virtual unsigned open(String) = 0;
    virtual void close(unsigned) = 0;
    virtual unsigned read(void *, unsigned, unsigned) = 0;
    virtual unsigned write(void *, unsigned, unsigned) = 0;
    virtual void create(String) = 0;
    virtual void del(String) = 0;
    virtual FileInfo info(String) = 0;
    virtual void cmd(String) = 0;
};

#endif