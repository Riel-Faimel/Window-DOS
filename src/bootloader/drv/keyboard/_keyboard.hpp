#include "keyboard.hpp"
#include <hardlib/x86/port.h>
#include <drv/screen/screen_srv.hpp>
#include <DescrpTable/IDNT.hpp>
#include <DescrpTable/idt.hpp>

#define DATA 0x60
#define CMD 0x64