#include "fat16.hpp"
#include "fat_struc.hpp"
#include <lib/cppstdlib/string>
#include <global/new.hpp>
#include <lib/cppstdlib/utility>

#ifdef _DEBUG
#include <drv/screen/screen_srv.hpp>
#endif

u8 calc_checksum(char *name) {
    u8 sum = 0;
    for (int i = 0; i < 11; i++) {
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + name[i];
    }
    return sum;
}

struct CacheObj {
    u32 sector;
    u8 data[512];
    bool dirty;
};

template<typename T>
class SectorCache {
    rtl::list<CacheObj> m_cache;
    size_t m_max = 8;
    
    CacheObj* find(u32 sector) {
        for (auto& e : m_cache) if (e.sector == sector) return &e;
        return nullptr;
    }
    CacheObj* load(u32 sector) {
        if (m_cache.get_size() >= m_max) {
            CacheObj &it = *m_cache.begin();
            if (it.dirty) disk_write(it.data, it.sector, 1);
            m_cache.remove(&it);
        }
        CacheObj *e = m_cache.space_append();
        new (e) CacheObj{sector, {}, false};
        if (disk_read(&e->data, sector, 1) == -1) {
            m_cache.remove(e);
            return nullptr;
        }
        return e;
    }
    inline unsigned disk_read(void* b, u32 s, u32 c) { return static_cast<T *>(this)->disk_read(b, s, c); }
    inline unsigned disk_write(void* b, u32 s, u32 c) { return static_cast<T *>(this)->disk_write(b, s, c); }
protected:
    SectorCache() = default;
    ~SectorCache() {
        for (auto &e : m_cache)
        if (e.dirty) disk_write(e.data, e.sector, 1);
    }

    u8* sector(u32 sector, bool dirty = false) {
        CacheObj* e = find(sector);
        if (!e) e = load(sector);
        if (e && dirty) e->dirty = true;
        return e ? e->data : nullptr;
    }
    void invalidate(u32 sector) {
        for (auto &it : m_cache)
        if (it.sector == sector) {
            if (it.dirty) disk_write(it.data, it.sector, 1);
            m_cache.remove(&it);
            return;
        }
    }
    void set_max_cache(size_t max) { m_max = max; }
};

template<typename T>
class SectorView : public SectorCache<T> {
    using SectorCache<T>::sector;
    using SectorCache<T>::invalidate;
    using SectorCache<T>::set_max_cache;

    size_t sector_start;
    size_t sector_count;

    u32 sector_at (size_t offset) {
        return offset / 512u + sector_start;
    }
    rtl::pair<u8 *, bool>ptr(size_t offset) {
        auto sec = sector_at(offset);
        if (sec >= sector_start + sector_count) return {nullptr, true};
        u32 off = offset % 512u;
        return {sector(sec, true) + off, false};
    }
protected:
    SectorView(): SectorCache<T>{} {}

    void init(size_t start, size_t count, size_t max_cache = 4) {
        sector_start = start;
        sector_count = count;
        set_max_cache(max_cache);
    }
    rtl::pair<u8, bool> get(size_t offset) {
        auto [p, error] = ptr(offset);
        return {(u8)(p ? *p : 0), error};
    }
    bool set(size_t offset, u8 v) {
        auto [p, error] = ptr(offset);
        if (error) return true;
        else {
            *p = v;
            return false;
        }
    }
};

template<typename T>
class FATTable : public SectorView<FATTable<T>> {
    using SectorView<FATTable<T>>::get;
    using SectorView<FATTable<T>>::set;
    size_t sector_count;
    
protected:
    FATTable(): SectorView<FATTable<T>>{} {}
    ~FATTable(){
        for (u16 i = 0; i < sector_count * 512 / 2; i++) {
            set_fat(i, 0);
        }
    }
    void init(u16 start, u16 max_cluster, size_t max_cache = 4) {
        SectorView<FATTable<T>>::init(start, max_cluster, max_cache);
    }
    
    rtl::pair<u16, bool> get_fat(u16 cluster) {
        if (cluster == 0) return {0, true};
        auto [high, err1] = get(cluster * 2 + 1);
        auto [low, err2] = get(cluster * 2); // cluster as offset in FAT
        if (err1 || err2) return {0, true};
        return {static_cast<u16>((high << 8) | low), false};
    }
    bool set_fat(u16 cluster, u16 value) {
        u8 low = static_cast<u8>(value & 0xFF);
        u8 high = static_cast<u8>((value >> 8) & 0xFF);
        auto err1 = set(cluster * 2, low);
        auto err2 = set(cluster * 2 + 1, high);
        return !err1 && !err2;
    }
    rtl::pair<u16, bool> alloc_cluster() {
        for (u16 offset = 4; ; offset += 2) {
            auto [low, errl] = get(offset);
            auto [high, errh] = get(offset + 1);
            if (errl || errh) return {0, true};
            if (!high && !low) {
                set(offset, 0xFF);
                set(offset + 1, 0xFF);
                return {offset / 2, false};
            }
        }
    }
    void free_chain(u16 cluster) {
        while (cluster >= 2) {
            auto [next, err] = get_fat(cluster);
            set_fat(cluster, 0);
            if (err) break;
            cluster = next;
        }
    }
public:
    inline unsigned disk_read(void* b, u32 s, u32 c) { return static_cast<T *>(this)->disk_read(b, s, c); }
    inline unsigned disk_write(void* b, u32 s, u32 c) { return static_cast<T *>(this)->disk_write(b, s, c); }
};

template<typename T>
class ClusterView : public SectorCache<T> {
    using SectorCache<T>::sector;
    using SectorCache<T>::invalidate;
    using SectorCache<T>::set_max_cache;

    u16 cluster_at(u16 cluster_start, size_t offset) {
        u16 clu = cluster_start;
        while (clu < 0xFFF8) {
            if (offset < (cluster_size(clu) * 512u)) return clu;
            offset -= (cluster_size(clu) * 512u);
            auto [next_clu, err] = get_fat(clu);
            if (err) break;
            clu = next_clu;
        }
        return 1; // invalid cluster
    }
    inline size_t cluster_size(u16 c) { return static_cast<T *>(this)->cluster_size(c); }
    inline u32 cluster_to_sector(u16 c) { return static_cast<T *>(this)->cluster_to_sector(c); }
    inline rtl::pair<u16, bool> get_fat(u16 cluster) { return static_cast<T *>(this)->get_fat(cluster); }

protected:
    ClusterView(): SectorCache<T>{} {}

    void init(size_t cache_size = 4) {
        set_max_cache(cache_size); 
    }
    rtl::pair<u8 *, bool>ptr(u16 cluster, size_t offset) {
        auto c = cluster_at(cluster, offset);
        if (c == 1) return {0, true}; // invaild cluster
        u32 off = offset % (cluster_size(c) * 512u);
        return {sector(cluster_to_sector(c) + off / 512u, true) + off % 512, false};
    }
    rtl::pair<u8, bool> get(u16 cluster, size_t offset) {
        auto [p, err] = ptr(cluster, offset);
        if (err) return {0, true};
        return {*p, false};
    }
    bool set(u16 cluster, size_t offset, u8 v) {
        auto [p, err] = ptr(cluster, offset);
        if (err) return true;
        *p = v;
        return false;
    }
    rtl::pair<DIR *, bool> dirptr(u16 cluster, size_t offset) {
        auto [p,e] = ptr(cluster, offset * sizeof(DIR));
        return {(DIR *)p, e};
    }
};

template<typename T>
class DirResolv : public ClusterView<DirResolv<T>> {
    using ClusterView<DirResolv<T>>::init;
    using ClusterView<DirResolv<T>>::ptr;
    using ClusterView<DirResolv<T>>::get;
    using ClusterView<DirResolv<T>>::set;
    using ClusterView<DirResolv<T>>::dirptr;

    size_t rootdir_sec_start = 0;
    size_t rootdir_sec_num = 0;

    rtl::pair<DIR*, bool> find_dir_entry(u16 cluster, String &&filename) {
        u16 index = 0;
        while (cluster != 0 || index < 8) {
        // this condition means if root dir, only tranvese 512 entries, 
        // otherwise traverse until the end of cluster chain
            if (!match_entry(cluster, index, filename)) {
                return dirptr(cluster, index);
            }
        }
        return {nullptr, true};
    }

    bool match_entry(u16 cluster, u16& index, const String &filename) {
        auto [entry, err] = dirptr(cluster, index);
        if (err) return true;

        auto is_LFN_entry = [](DIR* e) {
            return (
                e->LFN.attribute.dir == 1 && 
                e->LFN.attribute.read_only == 1 && 
                e->LFN.attribute.system == 1 &&
                e->LFN.attribute.hidden == 1
            );
        };
        
        // LFN
        if (is_LFN_entry(entry)) {
            String long_name;
            u8 checksum = 0;
            
            while (true) {
                auto [e, err] = dirptr(cluster, index);
                if (err) return false;
                if (e->_8_3FN.name[0] == 0xE5) {
                    index++;
                    continue;
                }
                if (!is_LFN_entry(e)) break;
                if (e->LFN.order_index & 0x40) {
                    checksum = e->LFN.checksum;
                }
                for (int i = 0; i < 5; i++) {
                    u16 ch = e->LFN.first_name[i];
                    if (ch == 0) break;
                    long_name += (ch < 0x80) ? static_cast<char>(ch) : '?';
                }
                for (int i = 0; i < 6; i++) {
                    u16 ch = e->LFN.next_name[i];
                    if (ch == 0) break;
                    long_name += (ch < 0x80) ? static_cast<char>(ch) : '?';
                }
                for (int i = 0; i < 2; i++) {
                    u16 ch = e->LFN.final_name[i];
                    if (ch == 0) break;
                    long_name += (ch < 0x80) ? static_cast<char>(ch) : '?';
                }
                index++;
            }
            
            auto [sfn, err] = dirptr(cluster, index);
            if (err) return true;
            
            if (calc_checksum(sfn->_8_3FN.name) != checksum) {
                index++; // failed
                return true;
            }
            
            if (long_name == filename) return false;
            index++;
            return true;
        }
        
        if (entry->_8_3FN.name[0] == 0x00 || entry->_8_3FN.name[0] == 0xE5) {
            index++;
            return true;
        }
        
        String short_name;
        for (auto ch : entry->_8_3FN.name) {
            if (ch == ' ') break;
            short_name += ch;
        }
        if (entry->_8_3FN.ext[0] != ' ') {
            short_name += '.';
            for (auto ch : entry->_8_3FN.ext) {
                short_name += ch;
            }
        }
        
        if (short_name == filename) return false;
        index++;
        return true;
    }

protected:
    DirResolv(): ClusterView<DirResolv<T>>{} {}
    ~DirResolv(){}
    void init(size_t rootdir_start, size_t rootdir_size, size_t cache_size = 4) {
        rootdir_sec_start = rootdir_start;
        rootdir_sec_num = rootdir_size;
        ClusterView<DirResolv<T>>::init(cache_size);
    }

    rtl::pair<DIR*, DirResolvError> resolv_dir(String path) {
        path.get_word('\\');
        if (path.empty()) return {dirptr(0, 0).key, DirResolvError::NoError};

        auto [entry, err] = find_dir_entry(0, path.get_word('\\'));
        if (err) return {nullptr, DirResolvError::NoSuchFile};

        while (!path.empty()) {
            if (!(entry->_8_3FN.attribute.dir)) return {nullptr, DirResolvError::NotADir};
            auto [next_entry, next_err] = find_dir_entry(
                entry->_8_3FN.first_cluster_low,
                path.get_word('\\')
            );
            if (next_err) return {nullptr, DirResolvError::NoSuchFile};
            entry = next_entry;
        }
        return {entry, DirResolvError::NoError};
    }
public:
    inline unsigned disk_read(void* b, u32 s, u32 c) { return static_cast<T *>(this)->disk_read(b, s, c); }
    inline unsigned disk_write(void* b, u32 s, u32 c) { return static_cast<T *>(this)->disk_write(b, s, c); }
    inline u32 cluster_to_sector(u16 c) { 
        if (c == 0) return rootdir_sec_start;
        if (c == 1) return (u32)-1; // invalid cluster
        return static_cast<T *>(this)->cluster_to_sector(c); 
    }
    inline size_t cluster_size(u16 c) {
        if (c == 0) return rootdir_sec_num;
        if (c == 1) return 0; // invalid cluster
        return static_cast<T *>(this)->cluster_size();
    }
    inline rtl::pair<u16, bool> get_fat(u16 cluster) { return static_cast<T *>(this)->get_fat(cluster); }
};

class FAT16 final : public Cluster, public FATTable<FAT16>, public DirResolv<FAT16> {
public:
    enum STATUS {
        RAW,
        UNFORMAT,
        FORMAT,
    }status;

// disk info
    LogicalDisk *part;
    Cluster_Info info_ = { Cluster_Info::Type::FAT16, 0, 0 };
    unsigned sec_per_clu; // sectors num
    unsigned clu0_sec_offset; // sector offset
    unsigned bytes_per_sector;
public:
    FAT16(LogicalDisk *part);
    ~FAT16();

    unsigned read(void *, unsigned, unsigned, unsigned) override;
    unsigned write(void *, unsigned, unsigned, unsigned) override;
    unsigned open(String filename) override;
    unsigned close(unsigned ) override;
    unsigned create(String) override;
    unsigned delet(String) override;
    void *mmap(String, void *) override;
    Cluster_Info* info(String) override;
    unsigned cmd(unsigned, String, void *argv, unsigned argc) override;
private:
    void set_filesystem_name(char *name);
public:
    using FATTable<FAT16>::get_fat;
    u32 cluster_to_sector(u16 cluster);
    size_t cluster_size() { return sec_per_clu; }
    unsigned disk_read(void *, unsigned, unsigned);
    unsigned disk_write(void *, unsigned, unsigned);
};