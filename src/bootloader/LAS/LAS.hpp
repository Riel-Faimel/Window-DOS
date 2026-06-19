#ifndef __bootloader_LAS_LAS_hpp
#define __bootloader_LAS_LAS_hpp

#include "LAS_.hpp"
#include <global/type.hpp>
#include <interface/cluster.hpp>
#include <part-fs/MBR/MBR.hpp>
#include <global/WIN.hpp>
#include <PDS/pds.hpp>
#include <cppstdlib/string>
#include <TL/container>

class LAS {
public:
    struct DriveInfo{
        Cluster *driver;
        /**
         * also partition manager or file system
         */
        String driver_letter;
        unsigned cid;
    };
    rtl::array<DriveInfo> space;

    enum Handle_Mode : unsigned char {
        Read = 0x01,
        Write = 0x02,
        Exec = 0x04,
    };
    struct Handle {
        unsigned ID;
        unsigned file_handle; 
        String path_buf;
        u8 handle_mode;
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
    void regist(Cluster *, String = String{});

    /**
     * read/write(Window, begin byte, byte nums) for read by sectors
     */
    unsigned read(_WIN&, unsigned, unsigned);
    unsigned write(_WIN&, unsigned, unsigned);
    /**
     * open(Window, String "A:\") for open a device also path on it
     */
    unsigned open(_WIN&, String, u8 = Read|Write);
    unsigned close(_WIN&);
    /**
     * creat/del(Window, String path) for files or folders, 
     * folder as file in this way
     */
    unsigned create(_WIN&, String);
    unsigned del(_WIN&, String);
    Cluster_Info info(_WIN&, String);
    unsigned cmd(_WIN&, unsigned, String);
};

#endif