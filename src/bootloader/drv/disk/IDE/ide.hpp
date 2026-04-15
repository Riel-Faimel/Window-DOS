#ifndef __bootloader_drv_disk_IDE_ide_hpp
#define __bootloader_drv_disk_IDE_ide_hpp
#include <IDT_L/idt.hpp> 
#include <drv/disk/disk.hpp>
class IDE_DISK;

class IDE_Channal {
public:
    enum class Channal{
        Master_Channel = 0x1F0,
        Slave_Channel = 0x170
    };
    IDE_Channal(IDE_DISK& master, IDE_DISK& slave, Channal chan, IDT& idt); //use strange

private:
    Channal chan;
    void read(unsigned short *buf, unsigned LBA, unsigned char count, DISK_INFO *info);
    void read_PIO_LBA(unsigned short *buf, unsigned LBA, unsigned char count, u8 dev);
    void read_PIO_CHS(unsigned short *buf, unsigned LBA, unsigned char count, u8 dev);

    void write(unsigned short *buf, unsigned LBA, unsigned char count, DISK_INFO *info);
    void write_PIO_LBA(unsigned short *buf, unsigned LBA, unsigned char count, u8 dev);
    void write_PIO_CHS(unsigned short *buf, unsigned LBA, unsigned char count, u8 dev);
    /**
     * from Linux2.6.32.1
     * include/linux/ata.h
     */
    struct ata_taskfile {
        unsigned long	flags;		/* ATA_TFLAG_xxx */
        u8			protocol;	/* ATA_PROT_xxx */
        u8			ctl;		/* control reg */
        u8			hob_feature;	/* additional data */
        u8			hob_nsect;	/* to support LBA48 */
        u8			hob_lbal;
        u8			hob_lbam;
        u8			hob_lbah;
        u8			feature;
        u8			nsect;
        u8			lbal;
        u8			lbam;
        u8			lbah;
        u8			device;
        u8			command;	/* IO operation */
    };
    void ata_sff_tf_read(ata_taskfile&);
    void ata_sff_tf_load(ata_taskfile &tf);
    void ata_sff_exec_command(ata_taskfile &tf);
    friend class IDE_DISK;
};

class IDE_DISK : public DISK_{
    friend class IDE_Channal;
    friend void LoaderMain();
    enum class Device{
        Master_Device = 0xE0,
        Slave_Device = 0xF0,
    };

    IDE_Channal *lock;
    DISK_INFO info_;
    IDE_DISK(Device dev, IDT &idt, IDE_Channal *);
public:
    bool exist;
    IDE_DISK();
    ~IDE_DISK() = default;

    void check();
    void read(unsigned short *buf, unsigned LBA, unsigned char count);
    void write(unsigned short *buf, unsigned LBA, unsigned char count);
    DISK_INFO *info();
    
    IDE_DISK& operator=(IDE_DISK&&);
    IDE_DISK& operator=(IDE_DISK&) = delete;
    IDE_DISK& operator=(const IDE_DISK&) = delete;
};
#endif