#ifndef __bootloader_DLS_DLS_hpp
#define __bootloader_DLS_DLS_hpp

/**
 * DLS (Driver Letter Space) is a space for all  file systems
 * it is manage and give call to file system
 */

#include "DLS_.hpp"
#include <global/type.hpp>
#include <interface/dev_cls/cluster.hpp>
//#include <part-fs/MBR/MBR.hpp>
#include <global/WIN.hpp>
#include <PDS/pds.hpp>
#include <cppstdlib/string>
#include <TL/container>

class DLS {
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
        unsigned count;
        u8 handle_mode;
    };
    unsigned did_count = 1;
    /**
     * skip A:
     * it is only for system disk
     */
public:
    DLS();
    /**
     * regist an FS mode
     * called by FS mode in initial function
     */
    void regist(Cluster *, String = String{});

    /**
     * read/write(Window, begin byte, byte nums) for read by sectors
     */
    unsigned read(WinHandle&, unsigned, unsigned);
    unsigned write(WinHandle&, unsigned, unsigned);
    /**
     * open(Window, String "A:\") for open a device also path on it
     */
    unsigned open(WinHandle&, String, u8 = Read|Write);
    unsigned close(WinHandle&);
    /**
     * creat/del(Window, String path) for files or folders, 
     * folder as file in this way
     */
    unsigned create(WinHandle&, String);
    unsigned del(WinHandle&, String);
    /**
     * Info(Window, String path) for get info of a file or folder or device
     */
    Cluster_Info info(WinHandle&, String);
    unsigned cmd(WinHandle&, unsigned, String, void *argv, unsigned argc);
    /**
     * mmap(Window, String) for map handling object to memory
     */
    void *mmap(WinHandle, String, void *);
};

#endif