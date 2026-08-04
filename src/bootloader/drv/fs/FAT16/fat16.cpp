#include "_fat16.hpp"

FAT16::FAT16(
    LogicalDisk *part_init, 
    bool force_format, 
    unsigned int cluster_index
):part(part_init), cluster_size(1<<cluster_index), 
status(RAW){
    BPB bpb;
    part->read(bpb.buf, 0, 0, 1);
    if(
        bpb.boot_sector_sign == 0xAA55 &&
        bpb.root_entries == 512
    ){
        cluster_size = bpb.sectors_per_cluster;
        if(bpb.fat_size_16 > 0){
            status = FORMAT; //has BPB
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

FORMATED:
    /**
     * TODO:
     * new FAT table in memory
     * read FAT table from disk
     * check FAT1, FAT2
     * 
     * if error: back to FAT2
     */
    clu2blk = (bpb.reserved_sectors + bpb.fat_size_16 * bpb.num_fats + (bpb.root_entries * 32 + bpb.bytes_per_sector - 1) / bpb.bytes_per_sector) - 2 * cluster_size;
    dir_entries = bpb.root_entries;
    FAT_table = new unsigned short [bpb.fat_size_16 * 256];
    part->read(FAT_table, bpb.reserved_sectors, 0, bpb.fat_size_16);
    root_dir = (DIR *)new unsigned char [bpb.root_entries * 32];
    rootdir_sector_num = bpb.reserved_sectors + bpb.num_fats * bpb.fat_size_16;
    part->read(root_dir, rootdir_sector_num, 0, (bpb.root_entries + 15) / 16);
    //for(unsigned i = 0;i < 512;i++) { print_hex(reinterpret_cast<unsigned char *>(root_dir)[i], false);print_char(' '); }
    return;

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
    
    auto info = part->info({});

    bpb.bytes_per_sector = 512;//fixed
    bpb.sectors_per_cluster= 1 << cluster_index;
    bpb.reserved_sectors = 1;
    bpb.num_fats = 2;
    bpb.root_entries = 512; //fixed
    bpb.total_sectors_16 = (info->total_bytes > 65536) ? 0 : info->total_bytes;
    bpb.medis_descriptor = 0xF8; //hdd
    bpb.fat_size_16 = 0;
    bpb.sectors_per_track = 63; //hdd
    bpb.num_heads = 16;
    bpb.total_sectors_32 = (info->total_bytes < 65536) ? 0 : info->total_bytes;
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
    part->write(bpb.buf, 0, 0, 1);
    status = UNFORMAT;
    if(force_format){
        format();
        goto FORMATED;
    }
    return;
}

void FAT16::format(){
    BPB bpb;
    auto info = part->info({});
    part->read(bpb.buf, 0, 0, 1);

    unsigned S = cluster_size;
    unsigned T = info->total_bytes;
    unsigned R, F, D, C;
    R = 1;
    F = 1;
    unsigned F_ = F;
    unsigned R_ = R;
    while(true){
        F_ = F;
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
    part->write(bpb.buf, 0, 0, 1);
    
    unsigned short fat[256];
    for(int i = 0;i < 256;i++) fat[i] = 0;
    fat[0] = 0xFFF8;
    fat[1] = 0xFFFF;
    part->write(fat, 0, R, 1);
    part->write(fat, 0, R + F, 1);
    fat[0] = 0;
    fat[1] = 0;
    for(unsigned i = 1;i < F;i++){
        part->write(fat, 0, R + i, 1);
        part->write(fat, 0, R + F + i, 1);
    }
    unsigned char *root = reinterpret_cast<unsigned char *>(fat);
    for(int i = 0;i < 11;i++)root[i] = bpb.volume_label[i];
    root[11] = 0x08;
    part->write((unsigned short *)root, 0, R + 2*F, 1);
    for(int i = 0;i <= 11;i++)root[i] = 0;
    for(int i = 1;i < 32;i++){
        part->write((unsigned short *)root, 0, R + 2 * F + i, 1);
    }
    
    status = FORMAT;
    return;
}

void FAT16::set_filesystem_name(char *name){
    BPB bpb;
    part->read(bpb.buf, 0, 0, 1);
    for(unsigned char i = 0;i < 11;i++){
        bpb.volume_label[i] = name[i];
    }
    part->write(bpb.buf, 0, 0, 1);
    kprint("set name: ");kprint(name);
}

unsigned FAT16::open(String filename){
    if(status != FORMAT){ return -1; }
    if(root_dir == nullptr) { 
        return -1; // fresh root directory
    }
    filename.get_word('\\'); // skip the first '\' for root directory
    if (filename == String{}) return rootdir_sector_num;

    DIR *dir = root_dir;
    unsigned short dir_buf [cluster_size*256];
    unsigned cluster_id;
    u8 attr;

    if (auto this_filename = filename.get_word('\\');resolv_dir(dir, 512, this_filename) != 0) {
        // found
        cluster_id = dir->_8_3FN.first_cluster_low|(dir->_8_3FN.first_cluster_high << 16);
        //print_hex(cluster_id);print_char('\n');
        dir = reinterpret_cast<DIR *>(dir_buf);
        part->read(dir, cluster_id, 0, cluster_size);
    } else {
        // no such file
        return -1;
    }
    if (filename != String{})
    while(1) {
        auto thisname = filename.get_word('\\');
        attr = resolv_dir(dir, cluster_size*256/sizeof(DIR), thisname);
        if (!attr) { return -1; } // no such file
        // fresh cluster need to read and dir
        cluster_id = dir->_8_3FN.first_cluster_low|(dir->_8_3FN.first_cluster_high << 16);
        if (filename == String{}) break; // resolv done
        if (!(attr & attribute_choice::dir)) { return -1; }
        dir = reinterpret_cast<DIR *>(dir_buf);
        part->read(dir, cluster_id, 0, cluster_size);
    };
    return cluster_id;
}

u8 FAT16::resolv_dir(DIR *&dir, unsigned num, String dirname) {
    for(unsigned i = 0;i < num;i++) {
        auto item = dir[i]._8_3FN; // each directory item
        
        // directory end
        if (item.name[0] == 0x00) break;
        // delete file
        if ((u8)(item.name[0]) == 0xE5) continue;

        String itemname;
        if (item.attribute & long_filename) {
            auto re = &dir[i];
            for (;i < num;i++) {
                auto long_item = dir[i].LFN;
                for (auto uch : long_item.final_name) {
                    if (uch == 0) goto done;
                    itemname += (u8)uch;
                }
                for (auto uch : long_item.next_name) {
                    if (uch == 0) goto done;
                    itemname += (u8)uch;
                }
                for (auto uch : long_item.final_name) {
                    if (uch == 0) goto done;
                    itemname += (u8)uch;
                }
            };// handle long filename
            // read whole dir buf but not done
            return 0;
        done:
            //kprint(itemname);
            if (itemname != dirname) continue;

            dir = re;
            return (u8)item.attribute;
        } else {
            itemname = String{item.name, 8}.trim();
            auto extname = String{item.ext, 3}.trim();
            if (extname != String{}) itemname = itemname + '.' + extname;
            //kprint(itemname);
            if (dirname != itemname) continue;

            dir = &dir[i];
            return (u8)item.attribute;
        }
    }
    return 0;
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
        part->read(bpb.buf, 0, 0, 1);
        
        // 写回FAT表到两个FAT副本
        part->write(FAT_table, 0, bpb.reserved_sectors, bpb.fat_size_16);
        part->write(FAT_table, 0, bpb.reserved_sectors + bpb.fat_size_16, bpb.fat_size_16);
        
        // 写回根目录
        part->write(reinterpret_cast<unsigned short *>(root_dir), 0, bpb.reserved_sectors + 2 * bpb.fat_size_16, bpb.root_entries / 16);
        
        // 释放内存
        delete [] FAT_table;
        delete [] reinterpret_cast<unsigned short *>(root_dir);
    }
}

unsigned FAT16::read(void *buf, unsigned cluster_start, unsigned byte_from, unsigned byte_read) {

    unsigned start_sector = cluster_start * cluster_size + clu2blk + byte_from / 512;
    unsigned byte_offset = byte_from % 512;
    
    unsigned char *dest = static_cast<unsigned char *>(buf);
    unsigned bytes_copied = 0;
    
    if (byte_offset != 0) {
        unsigned char temp_buf[512];
        part->read(temp_buf, start_sector, 0, 1);
        
        unsigned first_copy = (512 - byte_offset < byte_read) ? 
                              (512 - byte_offset) : byte_read;
        
        for (unsigned j = 0; j < first_copy; j++) {
            dest[j] = temp_buf[byte_offset + j];
        }
        
        bytes_copied = first_copy;
        start_sector++;
    }
    
    if (bytes_copied < byte_read) {
        unsigned remaining = byte_read - bytes_copied;
        unsigned full_sectors = remaining / 512;
        
        if (full_sectors > 0) {
            part->read(dest + bytes_copied, start_sector, 0, full_sectors);
            bytes_copied += full_sectors * 512;
            start_sector += full_sectors;
        }
        
        if (bytes_copied < byte_read) {
            unsigned char temp_buf[512];
            part->read(temp_buf, start_sector, 0, 1);
            
            unsigned last_copy = byte_read - bytes_copied;
            for (unsigned j = 0; j < last_copy; j++) {
                dest[bytes_copied + j] = temp_buf[j];
            }
        }
    }
    
    return bytes_copied;
}

unsigned FAT16::write(void *, unsigned int, unsigned int, unsigned int){
    return 0;
}

unsigned FAT16::create(String) {
    return 0;
}

unsigned FAT16::delet(String){
    return 0;
}

Cluster_Info *FAT16::info(String) {
    return &info_;
}

unsigned FAT16::cmd(unsigned cmd_id, String param, void *, unsigned ){
    switch (cmd_id) {
    case 0:
        set_filesystem_name(param.c_str());
        return 0;
    
    default:
        return 0;
    }
}

unsigned FAT16::fat_map(unsigned itemid) const {
    return FAT_table[itemid];
}
    
unsigned FAT16::clu2sec_map(unsigned clu)const {
    return (clu-2) * cluster_size + clu2blk;
}

void *FAT16::mmap(String filepath, void * = nullptr) {
    ;
}
