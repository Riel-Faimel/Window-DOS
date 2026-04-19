#ifndef __bootloader_part_fs_FAT16_hpp
#define __bootloader_part_fs_FAT16_hpp
#include <part-fs/MBR/MBR_.hpp>
#include <lib/cppstdlib/string>

class FAT16{
public:
    enum class attribute_choice : unsigned char {
        read_only = 0b00000001,
        hidden = 0b00000010,
        system = 0b00000100,
        volume_ID = 0b00001000,
        dir = 0b00010000,
        archive = 0b00100000,
        long_filename = 0b00001111,
    };
#pragma pack(push, 1)
    union BPB{
        struct {    
            unsigned char jmp[3];
            unsigned char oem[8];

            unsigned short bytes_per_sector;
            unsigned char sectors_per_cluster;
            unsigned short reserved_sectors;
            unsigned char num_fats;
            unsigned short root_entries;
            unsigned short total_sectors_16;
            unsigned char medis_descriptor;
            unsigned short fat_size_16;
            unsigned short sectors_per_track;
            unsigned short num_heads;
            unsigned hidden_sectors;
            unsigned total_sectors_32;
            
            unsigned char drive_number;
            unsigned char reserved1;
            unsigned char boot_sign;
            unsigned volume_id;
            unsigned char volume_label[11];
            unsigned char fs_type[8];
            
            unsigned char boot_code[448];
            
            unsigned short boot_sector_sign;
        };

        unsigned short buf[256];
    };

    union DIR{
        struct {
            char name[8];
            char ext[3];
            attribute_choice attribute;
            unsigned char re_;
            unsigned char Creation_time_in_microsecond;
            unsigned short Creation_time_in_hour_minute_second;
            unsigned short Creation_Date;
            unsigned short Last_access_Date;
            unsigned short first_cluster_high = 0;
            unsigned short Last_modification_time;
            unsigned short Last_modification_date;
            unsigned short first_cluster_low;
            unsigned file_size;
        } _8_3FN;

        struct {
            unsigned char order_index;
            /**
             * orer index takes
             * |reserve 1 bit|end label 1 bit|index 5 bits|
             * to mark long filename entry
             */
            unsigned short first_name[5];
            /**
             * filled with 0xFF, 
             * next, final name is the same
             */
            attribute_choice attribute = attribute_choice::long_filename;
            unsigned char type; //zero for long filename
            unsigned char checksum;
            /**
             * create from 8.3 filename
             */
            unsigned short next_name[6];
            unsigned short zero = 0;
            unsigned short final_name[2];
        } LFN;
    };
#pragma pack(pop)

    enum STATUS {
        NO,
        RAW,
        UNFORMAT,
        FORMAT
    };

// disk info
    DISK_PART *part;
    unsigned part_id;
    unsigned cluster_size;
    unsigned char clu2blk;

// status machine
    STATUS status;
    String current_path;

// file allocation table
    unsigned short *FAT_table;

// root dir
    DIR *root_dir;
    unsigned dir_entries;
public:
    FAT16(
        DISK_PART *part, 
        unsigned part_id, 
        bool force_part = false, 
        bool force_format = false, 
        unsigned cluster_index = 3
    );

    ~FAT16();

    void format();

    unsigned open(String filename);
    unsigned lookup(String);
    void cd(String);
    void dir();

    void set_filesystem_name(char *name);
};

#endif