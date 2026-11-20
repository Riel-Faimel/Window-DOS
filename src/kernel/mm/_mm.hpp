#include "mm.hpp"
#include <global/macro.hpp>
#include <stdio>
#include <HL/generator>

#define MemClusterSize 4096
#define SuperBlockMagic 'RSBM'
#define PageHeadMagic 'RPHM'
#define ObjectHeadGuard 'HG'
#define ObjectTailGuard 'TG'
#define ObjectSingalGuard 'SP'

constexpr u16 size_classes[mem::SizeClassCount] = {
    16, 24, 32, 48, 64, 96, 128, 192, 256, 
    384, 512, 768, 1024, 1536, 2048, 3072, 4096
};