#ifndef __bootloader_LAS_LAS_hpp
#define __bootloader_LAS_LAS_hpp

#include <global/type.hpp>
#include <interface/cluster.hpp>
#include <part-fs/MBR/MBR.hpp>
#include <global/WIN.hpp>
#include <PDS/pds.hpp>
#include <cppstdlib/string>
#include <TL/container>

class LAS {
    struct DriveInfo{
        Cluster *driver;
        /**
         * also partition manager or file system
         */
        String driver_letter;
        unsigned cid;
    };
    rtl::array<DriveInfo> space;

    enum class Handle_Mode {
        read = 0x01,
        write = 0x02,
        exec = 0x04,
    };
    struct Handle {
        unsigned file_handle; 

        unsigned ID;
        String path_buf;
        Handle_Mode handle_mode;
        unsigned count;
    };
    unsigned did_count = 1;
    /**
     * skip A:
     * it is only for system disk
     */
public:
    LAS();
    /**
     * regist an FS mode
     * called by FS mode in initial function
     */
    void regist(Cluster *, String);

    /**
     * read/write(Window, begin byte, byte nums) for read by sectors
     */
    unsigned read(_WIN&, unsigned , unsigned);
    unsigned write(_WIN&, unsigned, unsigned);
    /**
     * open(Window, String "A:\") for open a device also path on it
     */
    unsigned open(_WIN&, String);
    unsigned close(_WIN&);
    /**
     * creat/del(Window, String path) for files or folders, 
     * folder as file in this way
     */
    unsigned creat(_WIN&, String);
    unsigned del(_WIN&, String);
    unsigned info(_WIN&);
    unsigned cmd(_WIN&, String);
};

extern LAS* linear_address_space;

#endif