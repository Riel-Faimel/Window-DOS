#pragma GCC optimize("Os")
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
    case DirResolvError::NoError:
        return entry->_8_3FN.first_cluster_low;
    case DirResolvError::NoSuchFile:
        return 1;
    case DirResolvError::NotADir:
        return 1; // cluster 1 is always invalid cluster
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

unsigned FAT16::read(void *buf, unsigned handle, unsigned byte, unsigned count) {
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

void *FAT16::mmap(String filename, void *nice_address) {
    if (nice_address) {
        // I not care [doge]
    }
    auto [file_entry, err] = resolv_dir(filename);
    if (err != DirResolvError::NoError) return nullptr;

    auto file_size = file_entry->_8_3FN.file_size;
    auto cluster_start = file_entry->_8_3FN.first_cluster_low;
    auto buf = new u8 [file_size];
    auto bytes_read = read(buf, cluster_start, 0, file_size);
    if (bytes_read != file_size) return nullptr;
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
        info_.type = Cluster_Info::Type::FAT16;
    }
}

Cluster *create_fat_fs(LogicalDisk *part) {
    return new FAT16(part);
}

#pragma GCC optimize("Os")