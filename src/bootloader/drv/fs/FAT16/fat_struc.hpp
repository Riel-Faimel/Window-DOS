#pragma pack(push, 1)
struct attribute_choice {
    u8 read_only : 1 = 0;
    u8 hidden : 1 = 0;
    u8 system : 1 = 0;
    u8 volume_ID : 1 = 0;
    u8 dir : 1 = 0;
    u8 archive : 1 = 0;
    u8 reserve : 2 = 0;
};

union BPB{
    struct {    
        unsigned char jmp[3];
        unsigned char oem[8];

        unsigned short bytes_per_sector;
        unsigned char sectors_per_cluster;
        unsigned short reserved_sectors; // from vLBA 0 to FAT1 start
        unsigned char fat_table_count;
        unsigned short root_entry_num;
        unsigned short total_sectors_16;
        unsigned char medis_type_descriptor;
        unsigned short sector_per_fat_table;
        unsigned short sectors_per_track;
        unsigned short num_heads;
        unsigned hidden_sectors; // from phyLBA 0 to vLBA 0
        unsigned total_sectors_32;
        
        unsigned char drive_number;
        unsigned char reserved2;
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
        attribute_choice attribute = {1,1,1,1};
        unsigned char type; //zero for long filename
        unsigned char checksum;
        /**
         * create from 8.3 filename
         */
        unsigned short next_name[6];
        unsigned short zero = 0;
        unsigned short final_name[2];

        bool get_name(String &re) {
            for (auto uch : first_name) {
                if (uch == 0) return true;
                re += (char)uch;
            }
            for (auto uch : next_name) {
                if (uch == 0) return true;
                re += (char)uch;
            }
            for (auto uch : final_name) {
                if (uch == 0) return true;
                re += (char)uch;
            }
            return false;
        }
    } LFN;
};
#pragma pack(pop)


enum class DirResolvError {
    NoError,
    NoSuchFile,
    NotADir,
};