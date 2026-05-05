#ifndef __bootloader_LAS_LAS_hpp
#define __bootloader_LAS_LAS_hpp

#define _BITS_32
#include <global/type.hpp>
#include <interface/cluster.hpp>
#include <global/WIN.hpp>
#include <cppstdlib/string>
#define __independent_lib_Using_template_container
#include <TL/container>

class LAS {
    struct ClusterInfo{
        String drive_letter;
        Cluster *drive;
        unsigned ID;
    };
    rtl::array<ClusterInfo> space;

    enum class Handle_Mode {
        read = 0x01,
        write = 0x02,
        exec = 0x04,
    };
    struct Handle {
        unsigned ID;
        String path_buf;
        Handle_Mode handle_mode;
        unsigned count;

        unsigned file_handle;
    };
public:
    LAS();
    /**
     * regist an FS mode
     * called by FS mode in initial function
     */
    void regist();

    /**
     * read/write(Window, begin LBA, end LBA) for read by sectors
     */
    virtual unsigned read(_WIN&, unsigned , unsigned);
    virtual unsigned write(_WIN&, unsigned, unsigned);
    virtual unsigned open(_WIN&, String);
    virtual unsigned close(_WIN&);
    virtual unsigned mkdir(_WIN&, const char *);
    virtual unsigned deldir(_WIN&, const char *);
    virtual unsigned creat(_WIN&, const char *);
    virtual unsigned del(_WIN&, const char *);
    virtual unsigned cmd(_WIN&, const char *);
};

extern LAS *linear_address_space;

#endif