/**
 * I mean, we should not #include this file into global headers
 * here defined _BITS_32, I do not sure if it goes to error
 * I mean, I gonna delete the define, you should keep that
 * size_t had define and then! you use this header but I can do 
 * any thing here!
 */
#ifndef __bootloader_drv_disk_IDE_ide_controller_init_hpp
#define __bootloader_drv_disk_IDE_ide_controller_init_hpp
/*
#ifndef _BITS_32
#define _BITS_32
#endif
#include <global/type.hpp>
*/
void init_IDE_controller(void* *, size_t *);
#endif