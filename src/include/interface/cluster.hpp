#ifndef __include_interface_bootoader_cluster_hpp
#define __include_interface_bootoader_cluster_hpp

class Cluster {
public:
    struct Cluster_Info {
        enum class Type{
            IDE_Disk_driver,
            SATA_Disk_driver,
            MBR_Part_manager,
            minFS,
            FAT16,
        } type;
    };
    /**
     * if disk driver realize 
     * read/write(buffer, LBA, byte_offset, byte_deal)
     * 
     * if File system realize
     * read/write(buffer, handle, byte_offset, byte_deal)
     */
    virtual void read(unsigned short *buf, unsigned, unsigned, unsigned) = 0;
    virtual void write(unsigned short *buf, unsigned, unsigned, unsigned) = 0;
    virtual Cluster_Info &info() = 0;

    virtual unsigned open(char *file_path);
    virtual void close(unsigned handle);
    virtual void create(char *);
    virtual void delet(char *);
    virtual void show();
    virtual void command(char *);
};

#endif