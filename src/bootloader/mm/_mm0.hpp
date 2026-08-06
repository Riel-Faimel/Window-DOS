#include "mm0.hpp"
#include <start/BIOSinfo.hpp>
#include <drv/screen/screen_srv.hpp>

#pragma pack(push, 1)
struct mmap_struc {
    u32 base_low;
    u32 base_high;
    u32 len_low;
    u32 len_high;
    enum Type : u32 {
        usable = 1,
        reserved = 2,
        ACPI = 3,
        NVS = 4,
        bad = 5,
    } type;
};
#pragma pack(pop)
inline mmap_struc *mmap_struc_base = (mmap_struc *)0x5000;

struct ptr2size_list_item{
    void *ptr;
    unsigned size;
};

class _ptr2size{
    friend class MemoryManager;
    ptr2size_list_item *list_address;
    unsigned list_size;

    _ptr2size(void *ptr, unsigned size);
    void regist(void *ptr, unsigned size);
    unsigned get_size(void *ptr);
    unsigned remove(void *ptr);
public:
    _ptr2size() = default;
};

extern _ptr2size reserved_space_for_ptr2size_list_of_mememory_manager;