#pragma GCC optimize("O2")
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "_fat16.hpp"

FAT16::FAT16(LogicalDisk *part_init):
FATTable{}, DirResolv{}, 
status{RAW}, part{part_init}{
    BPB bpb;
    part->read(bpb.buf, 0, 0, 1);
    if(
        bpb.boot_sector_sign == 0xAA55 &&
        bpb.root_entry_num == 512
    ){
        sec_per_clu = bpb.sectors_per_cluster;
        if(bpb.sector_per_fat_table > 0){
            status = FORMAT; //has BPB
        }else{
            status = UNFORMAT;
            return;
        }
    }else return;

// formated
    /**
     * TODO:
     * new FAT table in memory
     * read FAT table from disk
     * check FAT1, FAT2
     * 
     * if error: back to FAT2
     */
    auto rootdir_sector_start = bpb.reserved_sectors + bpb.fat_table_count * bpb.sector_per_fat_table;
    auto rootdir_sector_size = (bpb.root_entry_num * sizeof(DIR) + bpb.bytes_per_sector - 1) / bpb.bytes_per_sector;

    bytes_per_sector = bpb.bytes_per_sector;
    sec_per_clu = bpb.sectors_per_cluster;
    clu0_sec_offset = rootdir_sector_start + rootdir_sector_size - 2 * sec_per_clu;

    FATTable::init(bpb.reserved_sectors, bpb.sector_per_fat_table);
    DirResolv::init(rootdir_sector_start, rootdir_sector_size);
    return;
}

unsigned FAT16::open(String filename) {
    auto [entry, err] = resolv_dir(filename);
    switch (err) {
    case DirResolvException::NoException: {
        return (unsigned) (new FileInfo{
            ClusterInfo{
                .index_start {entry->_8_3FN.first_cluster_low},
                .index_num {entry->_8_3FN.file_size},
                .index_size {1}
            },
            filename, entry->_8_3FN.Creation_Date,
            entry->_8_3FN.Creation_time_in_hour_minute_second,
            entry->_8_3FN.Last_modification_date,
            entry->_8_3FN.Last_modification_time,
            entry->_8_3FN.Last_access_Date,
            entry->_8_3FN.attribute, 0
        });
    } break;
    case DirResolvException::FindFileFailed:
    case DirResolvException::AsDirectoryFailed:
    default: {
        LocalDebugInfo(cout << "Failed\n";)
        return (unsigned)-2;
    } break;
    case DirResolvException::ClusterMissFault:
    case DirResolvException::DirBrokenFault: {
        LocalDebugInfo(cout << "Fault\n";)
        return (unsigned)-1;
    } break;
    }    
}

unsigned FAT16::close(unsigned) {
    // no need to close
    return 0;
}

unsigned FAT16::create(String) {
    return 0;
}

unsigned FAT16::delet(String) {
    return 0;
}

unsigned FAT16::read(void *buf, unsigned h, unsigned byte, unsigned count) {
    u16 handle = ((FileInfo *)(h))->index_start;
    if (handle == 0 || handle == 1) return -1;
    auto total_sector_skip_num = byte / bytes_per_sector;
    auto in_sector_bytes_skip_num = byte % bytes_per_sector;

    auto cluster_skip_num = total_sector_skip_num / cluster_size();
    auto in_cluster_sector_skip_num = total_sector_skip_num % cluster_size();

    u16 reading_clu = handle;
    for (; cluster_skip_num > 0; cluster_skip_num--) {
        auto [next_clu, err] = get_fat(reading_clu);
        if (err) return -1;
        reading_clu = next_clu;
    }
    
    // head read
    auto start_sector = cluster_to_sector(reading_clu) + in_cluster_sector_skip_num;
    u32 bytes_read = 0;
    u8 *buf_ptr = static_cast<u8 *>(buf);

    if (in_sector_bytes_skip_num) {
        u8 tmp_buf[bytes_per_sector];
        disk_read(tmp_buf, start_sector, 1);
        u32 copy_len = min(bytes_per_sector - in_sector_bytes_skip_num, count);
        __builtin_memcpy(buf_ptr, tmp_buf + in_sector_bytes_skip_num, copy_len);
        buf_ptr += copy_len;
        bytes_read += copy_len;
        start_sector++;
    }

    // body read
    u32 remaining = count - bytes_read;
    u32 full_sectors = remaining / bytes_per_sector;
    u32 tail_bytes = remaining % bytes_per_sector;

    while (full_sectors > 0) {
        u32 sectors_in_cur_cluster = cluster_size() - (start_sector - cluster_to_sector(reading_clu));
        u32 sectors_to_read = min(full_sectors, sectors_in_cur_cluster);
        
        disk_read(buf_ptr, start_sector, sectors_to_read);
        buf_ptr += sectors_to_read * bytes_per_sector;
        bytes_read += sectors_to_read * bytes_per_sector;
        full_sectors -= sectors_to_read;
        start_sector += sectors_to_read;

        if (full_sectors > 0) {
            auto [next_clu, err] = get_fat(reading_clu);
            if (err) return -1;
            reading_clu = next_clu;
            start_sector = cluster_to_sector(reading_clu);
        }
    }

    // tail read
    if (tail_bytes > 0) {
        u8 tmp_buf[bytes_per_sector];
        disk_read(tmp_buf, start_sector, 1);
        __builtin_memcpy(buf_ptr, tmp_buf, tail_bytes);
        bytes_read += tail_bytes;
    }

    return bytes_read;
}

unsigned FAT16::write(void *buf, unsigned handle, unsigned byte, unsigned count) {
    return 0;
}

ClusterInfo *FAT16::info(unsigned, String) {
    return nullptr;
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

void *FAT16::mmap(unsigned h, void *nice_address) {
    auto fip = (FileInfo *)(h);
    auto file_size = fip->index_num;
    if (nice_address) {
        // I not care [doge]
    }
    auto buf = new u8 [file_size];
    auto bytes_read = read(buf, h, 0, file_size);
    if (bytes_read != file_size) {
        delete buf;
        return nullptr;
    }
    return buf;
}

u32 FAT16::cluster_to_sector(u16 cluster) {
    return cluster * sec_per_clu + clu0_sec_offset;
}
unsigned FAT16::disk_write(void *buf, unsigned sector, unsigned count) {
    return part->write(buf, sector, 0, count);
}
unsigned FAT16::disk_read(void *buf, unsigned sector, unsigned count) {
    return part->read(buf, sector, 0, count);
}

void FAT16::set_filesystem_name(char *name) {
    for (int i = 0; i < 8; i++) {
        if (name[i] == '\0') break;
    }
}

Cluster *create_fat_fs(LogicalDisk *part) {
    return new FAT16(part);
}

void format_fat_fs(
    LogicalDisk *part, size_t cluster_size
){
    BPB bpb;
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
    bpb.sectors_per_cluster= cluster_size;
    bpb.reserved_sectors = 1;
    bpb.fat_table_count = 2;
    bpb.root_entry_num = 512; //fixed
    bpb.total_sectors_16 = (info->index_num > 65536u) ? 0 : info->index_num;
    bpb.medis_type_descriptor = 0xF8; //hdd
    bpb.sector_per_fat_table = 0;
    bpb.sectors_per_track = 63; //hdd
    bpb.num_heads = 16;
    bpb.total_sectors_32 = (info->index_num < 65536u) ? 0 : info->index_num;
    bpb.drive_number = 0x80;
    bpb.reserved2 = 0; //fixed
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

    unsigned S = cluster_size;
    unsigned T = info->index_num; // total sectors
    unsigned R, F, D, C; // Reserved, FAT, DataStart, ClusterNum
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
    // auto cluster0sector_id = D / S;
    bpb.reserved_sectors = R;
    bpb.sector_per_fat_table = F;
    part->write(bpb.buf, 0, 0, 1);
    
    // clear FAT
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

    // set volume label
    unsigned char *root = reinterpret_cast<unsigned char *>(fat);
    for(int i = 0;i < 11;i++)root[i] = bpb.volume_label[i];
    root[11] = 0x08;
    part->write((unsigned short *)root, 0, R + 2*F, 1);
    for(int i = 0;i <= 11;i++)root[i] = 0;
    for(int i = 1;i < 32;i++){
        part->write((unsigned short *)root, 0, R + 2 * F + i, 1);
    }
    return;
}

#pragma GCC optimize("Os")