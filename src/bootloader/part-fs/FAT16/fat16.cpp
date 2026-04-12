#include "_fat16.hpp"

FAT16::FAT16(
    DISK_PART *part_init, 
    unsigned int part_id, 
    bool force_part, 
    bool force_format, 
    unsigned int cluster_index
):part(part_init), 
part_id(part_id), 
status(NO), 
cluster_size(1<<cluster_index),
FAT_table(nullptr){
    BPB bpb;
    if(part->get_system_id(part_id) == 0x06){
        part->set_block(part_id, 1);
        part->read(bpb.buf, part_id, 0, 1);
        if(
            bpb.boot_sector_sign == 0xAA55 &&
            bpb.bytes_per_sector == 512
        ){
            cluster_size = bpb.sectors_per_cluster;
            if(bpb.fat_size_16 > 0){
                status = FORMAT; //has BPB
                part->set_block(part_id, cluster_size);
                goto FORMATED;
            }else{
                status = UNFORMAT;
                if(force_format){
                    format();
                    goto FORMATED;
                }
                return;
            }
        }else {
            status = RAW;
            goto SET_BPB;
        };
    }else {
        screen->print(" <no part> ");
        status = NO;
        if(force_part){
            screen->print(" <force part> ");
            part->init_part(DISK_PART::System_ID::FAT16_, part_id);
            goto SET_BPB;
        }
        return ;
    }
SET_BPB:
    for(unsigned i = 0;i < 256;i++){
        bpb.buf[i] = 0;
    }
    bpb.jmp[0] = 0xEB;
    bpb.jmp[1] = 0x3C;
    bpb.jmp[2] = 0x90; //fixed
    
    for(unsigned i = 0;i < 8 && "WINDOS0.1"[i];i++){
        bpb.oem[i] = "WINDOS0.1"[i];
    }
    
    bpb.bytes_per_sector = 512;//fixed
    bpb.sectors_per_cluster= 1 << cluster_index;
    cluster_size = bpb.sectors_per_cluster;
    bpb.reserved_sectors = 1;
    bpb.num_fats = 2; //
    bpb.root_entries = 512; //fixed
    bpb.total_sectors_16 = (part->part_info[part_id].part_size > 65536) ? 0 : part->part_info[part_id].part_size;
    bpb.medis_descriptor = 0xF8; //hdd
    bpb.fat_size_16 = 0;
    bpb.sectors_per_track = 63; //hdd
    bpb.num_heads = 16;
    bpb.hidden_sectors = part->part_info[part_id].start;
    //screen->print(" <into setting> ");
    bpb.total_sectors_32 = (part->part_info[part_id].part_size < 65536) ? 0 : part->part_info[part_id].part_size;
    bpb.drive_number = 0x80;
    bpb.reserved1 = 0; //fixed
    bpb.boot_sign = 0x29; //extra, 0x28 for not extra
    bpb.volume_id = 0x20260213;
    
    for(unsigned i = 0;i < 11 && "Riel FAT16"[i];i++){
        bpb.volume_label[i] = "Riel FAT16"[i];
    }
    
    for(unsigned i = 0;i < 8 && "FAT16 "[i];i++){
        bpb.fs_type[i] = "FAT16 "[i];
    }
    
    for(unsigned i = 0;i < 448;i++){
        bpb.boot_code[i] = 0;
    }
    
    bpb.boot_sector_sign = 0xAA55;
    part->write(bpb.buf, part_id, 0, 1);
    status = UNFORMAT;
    if(force_format){
        format();
        goto FORMATED;
    }
    return;

FORMATED:
    /**
     * TODO:
     * new FAT table in memory
     * read FAT table from disk
     * check FAT1, FAT2
     * 
     * if error: back to FAT2
     */
    FAT_table = new unsigned short [bpb.fat_size_16];
    part->read(FAT_table, part_id, bpb.hidden_sectors + clu2blk, bpb.fat_size_16 + clu2blk);
    //some problem
    return;
}

void FAT16::format(){
    BPB bpb;
    part->set_block(part_id, 1);
    part->read(bpb.buf, part_id, 0, 1);

    unsigned S = cluster_size;
    unsigned T = part->part_info[part_id].part_size;
    unsigned R, F, D, C;
    R = 1;
    F = 1;
    unsigned F_ = F;
    unsigned R_ = R;
    while(true){
        F_ = F;
        if(!S){
            screen->print("div 0");
            return ;
        };
        if((T - R - 2 * F - 32) < 0) {
            screen->print("small");
            return ;
        }
        F = (T - R - 2 * F - 32 + 256 * S - 1)/(256 * S);
        if(F != F_)continue;
        R_ = R;
        R = (2 * F + 32) % S ? S - ((2 * F + 32) % S) : 1;
        if(R != R_)continue;
        break;
    }
    D = R + 2 * F + 32;
    C = (T - D) / S;
    clu2blk = D / S;
    bpb.reserved_sectors = R;
    bpb.fat_size_16 = F;
    part->write(bpb.buf, part_id, 0, 1);
    
    unsigned short fat[256];
    for(int i = 0;i < 256;i++) fat[i] = 0;
    fat[0] = 0xFFF8;
    fat[1] = 0xFFFF;
    part->write(fat, part_id, R, 1);
    part->write(fat, part_id, R + F, 1);
    fat[0] = 0;
    fat[1] = 0;
    for(int i = 1;i < F;i++){
        part->write(fat, part_id, R + i, 1);
        part->write(fat, part_id, R + F + i, 1);
    }
    unsigned char *root = reinterpret_cast<unsigned char *>(fat);
    for(int i = 0;i < 11;i++)root[i] = bpb.volume_label[i];
    root[11] = 0x08;
    part->write((unsigned short *)root, part_id, R + 2*F, 1);
    for(int i = 0;i <= 11;i++)root[i] = 0;
    for(int i = 1;i < 32;i++){
        part->write((unsigned short *)root, part_id, R + 2 * F + i, 1);
    }
    
    part->set_block(part_id, S);
    screen->print(" <done> ");
    status = FORMAT;
    return;
}

void FAT16::set_filesystem_name(char name[11]){
    BPB bpb;
    part->set_block(part_id, 1);
    part->read(bpb.buf, part_id, 0, 1);
    for(unsigned char i = 0;i < 11;i++){
        bpb.volume_label[i] = name[i];
    }
    part->write(bpb.buf, part_id, 0, 1);
    part->set_block(part_id, cluster_size);
    screen->print("set name: ");screen->print(name);
}

unsigned int FAT16::open(const char *path){}

FAT16::~FAT16(){
    /**
     * TODO:
     * write back FAT table
     * write back BPB
     */
    ;
}