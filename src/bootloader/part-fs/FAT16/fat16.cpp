#include "_fat16.hpp"

FAT16::FAT16(
    DISK_PART *part_init, 
    unsigned int part_id_, 
    bool force_part, 
    bool force_format, 
    unsigned int cluster_index
):part(part_init), 
part_id(part_id_), 
status(NO), 
cluster_size(1<<cluster_index){
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
        //screen->print(" <no part> ");
        status = NO;
        if(force_part){
            //screen->print(" <force part> ");
            part->init_part(DISK_PART::System_ID::FAT16_, part_id);
            goto SET_BPB;
        }
        return;
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
    dir_entries = bpb.root_entries;
    FAT_table = new unsigned short [bpb.fat_size_16];
    part->read(FAT_table, part_id, bpb.hidden_sectors + clu2blk, bpb.fat_size_16 + clu2blk);
    root_dir = reinterpret_cast<DIR *>(new unsigned short [bpb.root_entries * 32]);
    part->read(reinterpret_cast<unsigned short *>(root_dir), part_id, bpb.hidden_sectors + clu2blk * 2, bpb.root_entries / 16);
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
    //screen->print(" <done> ");
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

unsigned FAT16::open(String filename){
    // 打开文件：根据文件名查找根目录中的条目，返回起始簇号
    if(status != FORMAT){
        return 0; // 未格式化，无法打开
    }
    
    // 解析文件名到8.3格式
    char name[9]; // 8个字符，空格填充
    char ext[4]; // 3个字符，空格填充
    
    // 查找点的位置
    unsigned int dot_pos = filename.length();
    for(unsigned int i = 0; i < filename.length(); i++){
        if(filename[i] == '.'){
            dot_pos = i;
            break;
        }
    }
    
    if(dot_pos < filename.length()){
        // 有扩展名
        String base = filename.substr(0, dot_pos);
        String extension = filename.substr(dot_pos + 1);
        
        // 复制文件名，大写，截断到8字符
        for(unsigned int i = 0; i < 8 && i < base.length(); i++){
            char c = base[i];
            if(c >= 'a' && c <= 'z') c -= 32;
            name[i] = c;
        }
        
        // 复制扩展名，大写，截断到3字符
        for(unsigned int i = 0; i < 3 && i < extension.length(); i++){
            char c = extension[i];
            if(c >= 'a' && c <= 'z') c -= 32;
            ext[i] = c;
        }
    } else {
        // 无扩展名
        for(unsigned int i = 0; i < 8 && i < filename.length(); i++){
            char c = filename[i];
            if(c >= 'a' && c <= 'z') c -= 32;
            name[i] = c;
        }
    }
    
    // 在根目录中查找
    for(unsigned i = 0; i < dir_entries; i++){
        if(root_dir[i]._8_3FN.name[0] == 0xE5 || root_dir[i]._8_3FN.name[0] == 0x00){
            continue; // 跳过删除或空条目
        }
        
        // 比较名称和扩展名
        bool match = true;
        for(unsigned int j = 0; j < 8; j++){
            if(root_dir[i]._8_3FN.name[j] != name[j]){
                match = false;
                break;
            }
        }
        if(!match) continue;
        
        for(unsigned int j = 0; j < 3; j++){
            if(root_dir[i]._8_3FN.ext[j] != ext[j]){
                match = false;
                break;
            }
        }
        if(!match) continue;
        
        // 找到匹配，且不是目录（假设只打开文件）
        if((unsigned char)root_dir[i]._8_3FN.attribute & (unsigned char)attribute_choice::dir){
            continue; // 跳过目录
        }
        
        // 返回起始簇号
        return root_dir[i]._8_3FN.first_cluster_low;
    }
    
    return 0; // 未找到
}

unsigned FAT16::close(unsigned ){
    return 0;
}

FAT16::~FAT16(){
    /**
     * Thanks for Copilot
     */
    // 写回FAT表和根目录到磁盘
    if(status == FORMAT){
        BPB bpb;
        part->set_block(part_id, 1);
        part->read(bpb.buf, part_id, 0, 1);
        
        // 写回FAT表到两个FAT副本
        part->write(FAT_table, part_id, bpb.reserved_sectors, bpb.fat_size_16);
        part->write(FAT_table, part_id, bpb.reserved_sectors + bpb.fat_size_16, bpb.fat_size_16);
        
        // 写回根目录
        part->write(reinterpret_cast<unsigned short *>(root_dir), part_id, bpb.reserved_sectors + 2 * bpb.fat_size_16, bpb.root_entries / 16);
        
        // 释放内存
        delete [] FAT_table;
        delete [] reinterpret_cast<unsigned short *>(root_dir);
    }
}

/*
void FAT16::dir(){
    if(dealing_path == "/")for(unsigned i = 0;i < dir_entries;i++){
        if(root_dir[i]._8_3FN.attribute == attribute_choice::dir){
            char filename[13];
            u8 i_;
            u8 j;
            for(i_ = 0;i_ < 8;i_++){
                filename[i_] = root_dir[i]._8_3FN.name[i_];
            }
            filename[i_] = '.';
            i_++;
            for(j = 0;j < 3;j++){
                filename[i_ + j] = root_dir[i]._8_3FN.ext[j];
            }
            filename[i_ + j] = '\0';
            screen->print(filename);
            print_char('\n');
        }
    }
    else {
        screen->print(dealing_path);
    };
}

unsigned int FAT16::lookup(String dir_name){
    /**
     * Thanks for Copilot
     */
    /*
    // 查找目录：根据目录名查找根目录中的目录条目，返回起始簇号
    if(status != FORMAT){
        return -1; // 未格式化，无法查找
    }
    
    if(dir_name == "/"){
        return 0; // 根目录簇号为0
    }
    
    // 解析目录名到8.3格式（目录通常无扩展名）
    char name[9]; // 8个字符，空格填充
    
    // 复制目录名，大写，截断到8字符
    for(unsigned int i = 0; i < 8 && i < dir_name.length(); i++){
        char c = dir_name[i];
        if(c >= 'a' && c <= 'z') c -= 32; // 简单大写转换
        name[i] = c;
    }
    
    // 在根目录中查找
    for(unsigned i = 0; i < dir_entries; i++){
        if(root_dir[i]._8_3FN.name[0] == 0xE5 || root_dir[i]._8_3FN.name[0] == 0x00){
            continue; // 跳过删除或空条目
        }
        
        // 比较名称
        bool match = true;
        for(unsigned int j = 0; j < 8; j++){
            if(root_dir[i]._8_3FN.name[j] != name[j]){
                match = false;
                break;
            }
        }
        if(!match) continue;
        
        // 检查是否为目录
        if(root_dir[i]._8_3FN.attribute != attribute_choice::dir){
            continue; // 不是目录
        }
        
        // 返回起始簇号
        return root_dir[i]._8_3FN.first_cluster_low;
    }
    
    return -1; // 未找到
}
*/