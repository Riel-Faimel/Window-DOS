#include "_mm0.hpp"

MemoryManager::MemoryManager() {}

MemoryManager::MemoryManager(
    unsigned block_size_init, 
    unsigned total_blocks_init, 
    void *base_address_init, 
    bool *bitmap_init,
    unsigned size
):
block_size(block_size_init),
total_blocks(total_blocks_init),
base_address(base_address_init),
bitmap(bitmap_init){
    for (unsigned i = 0; i < boot_infomation->mmap_size;i++){
        print_hex(mmap_struc_base[i].base_high);print_hex(mmap_struc_base[i].base_low, false);print_char('|');
        print_hex(mmap_struc_base[i].len_high);print_hex(mmap_struc_base[i].len_low, false);print_char('|');
        print_hex(mmap_struc_base[i].type);print_char('\n');
    }
    memorymanager = this;
    alloclist = new(&reserved_space_for_ptr2size_list_of_mememory_manager) _ptr2size(alloc(sizeof(ptr2size_list_item) * size), size);
}

void *MemoryManager::alloc(size_t size){
    unsigned blkneed = (size / block_size) + 1;
    for(unsigned i = 0;i < total_blocks;i++){
        if(bitmap[i])continue;
        //find the first unused block
        for(unsigned j = i;j < total_blocks;j++){
            //search forward
            if(j - i + 1>= blkneed){ //find!
                for(unsigned full = i;full <= j;full++){
                    bitmap[full] = true;
                } // as used
                void *re = static_cast<unsigned char *>(base_address) + block_size * i;
                alloclist -> regist(re, size);
                return re;
            }else{
                if(bitmap[j]){ // ohh, free blocks is to small
                    i = j;
                    break;
                }
            }
        }
    }
    return NULL_PTR;
}

void MemoryManager::dealloc(void *ptr){
    unsigned size = alloclist->remove(ptr);
    if(size == 0xFFFFFFFF)return ;
    unsigned blkfree = (size + block_size - 1)/ block_size;
    unsigned blkstart = (static_cast<unsigned char *>(ptr) - static_cast<unsigned char *>(base_address))/block_size;
    for(unsigned free = 0;free < blkfree;free++){
        bitmap[blkstart + free] = false;
    }
}

inline _ptr2size::_ptr2size(void *ptr, unsigned int size):
list_address(static_cast<ptr2size_list_item *>(ptr)),
list_size(size){}

void _ptr2size::regist(void *ptr, unsigned int size){
    for(unsigned i = 0;i < list_size;i++){
        if(!list_address[i].ptr){
            list_address[i].ptr = ptr;
            list_address[i].size = size;
            return;
        };
    }
}

unsigned int _ptr2size::get_size(void *ptr){
    for(unsigned i = 0;i < list_size;i++){
        if(list_address[i].ptr == ptr)return list_address[i].size;
    };
    return 0xFFFFFFFF;
}

unsigned int _ptr2size::remove(void *ptr){
    for(unsigned i = 0;i < list_size;i++){
        if(list_address[i].ptr == ptr){
            unsigned re = list_address[i].size;
            list_address[i].ptr = NULL_PTR;
            list_address[i].size = 0;
            return re;
        }
    };
    return 0xFFFFFFFF;
}

MemoryManager *memorymanager;

void *operator new(size_t size){
    return memorymanager->alloc(size);
}

void *operator new[](size_t size){
    return memorymanager->alloc(size);
}

void *operator new(size_t, void *ptr){
    return ptr;
}

void operator delete(void *ptr){
    memorymanager->dealloc(ptr);
}

void operator delete(void *ptr, unsigned int){
    memorymanager->dealloc(ptr);
}

void operator delete[](void *ptr, unsigned){
    memorymanager->dealloc(ptr);
}

void operator delete[](void *ptr){
    memorymanager->dealloc(ptr);
}

_ptr2size reserved_space_for_ptr2size_list_of_mememory_manager;