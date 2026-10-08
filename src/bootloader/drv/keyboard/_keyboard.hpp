#include "keyboard.hpp"
#include <port.h>
#include <drv/screen/screen_srv.hpp>
#include <arch/x86/IDNT.hpp>
#include <id/id.hpp>

#define DATA 0x60
#define CMD 0x64