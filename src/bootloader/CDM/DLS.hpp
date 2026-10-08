#ifndef __bootloader_CDM_DLS_hpp
#define __bootloader_CDM_DLS_hpp

/**
 * DLS (Driver Letter Space) is a space for all  file systems
 * it is manage and give call to file system
 */

#include "DLS_.hpp"
#include <global/type.hpp>
#include <interface/dev_cls/cluster.hpp>
#include <global/WIN.hpp>
#include <PDS/pds.hpp>
#include <string>
#include <TL/container>

class DLS {
public:
    struct DriveInfo{
        Cluster *driver;
        unsigned cid;
        /**
         * also partition manager or file system
         */
        String driver_letter;
    };
    rtl::array<DriveInfo> space;

    struct Handle {
        unsigned ID;
        unsigned file_handle;
    };
    unsigned did_count = 1;
    /**
     * skip A:
     * B: for boot disk
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
    unsigned read(void *, Handle *, unsigned, unsigned);
    unsigned write(void *, Handle *, unsigned, unsigned);
    /**
     * start(Window, String "A:") for open a device only
     * open(Window, String "A:\") for open a device also path on it
     */
    unsigned start(WinHandle&, String);
    unsigned open(WinHandle&, String);
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
    ClusterInfo *info(Handle *, String);
    unsigned cmd(Handle *, unsigned, String, void *argv, unsigned argc);
    /**
     * mmap(Window, String) for map handling object to memory
     */
    void *mmap(Handle *, void *);
private:
    Cluster *search_disk(Handle *);
};

#endif