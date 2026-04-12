#include "mm.hpp"

struct ptr2size_list_item{
    void *ptr;
    unsigned size;
};

class _ptr2size{
    friend class mm;
    ptr2size_list_item *list_address;
    unsigned list_size;

    _ptr2size(void *ptr, unsigned size);
    void regist(void *ptr, unsigned size);
    unsigned get_size(void *ptr);
    unsigned xxx(void *ptr);
public:
    _ptr2size() = default;
};

extern _ptr2size reserved_space_for_ptr2size_list_of_mememory_manager;