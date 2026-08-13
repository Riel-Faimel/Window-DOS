#include "mm.hpp"
#include <drv/screen/screen_srv.hpp>
#include <TL/generator>

#define MemClusterSize 4096
#define SuperBlockMagic 'RSBM'
#define PageHeadMagic 'RPHM'
#define ObjectHeadGuard 'HG'
#define ObjectTailGuard 'TG'
#define ObjectSingalGuard 'SP'

extern PhysicalPage* physicalpage;

constexpr u16 size_classes[__window_dos_define_0x1_SIZE_CLASS_COUNT] = {
    16, 24, 32, 48, 64, 96, 128, 192, 256, 
    384, 512, 768, 1024, 1536, 2048, 3072, 4096
};