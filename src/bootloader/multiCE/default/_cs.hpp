#include "cs.hpp"
#include <drv/screen/screen_srv.hpp>
#include <multiCE/cpu.hpp>
#include <DescripTable/intdis.hpp>
#include <mm/mm.hpp>
#include <DescripTable/idt.hpp>

extern "C" void basic_time_handler_c();