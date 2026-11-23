#include "context.hpp"
#include "tcb.hpp"
#include <global/new.hpp>
#include <id/id.hpp>
#include <mm/mm.hpp>
#include <global/macro.hpp>

extern "C" {
    void _Sexit();
    void _Sint_sche();
    void basic_time_handler();
}
namespace {
constexpr size_t thread_stack_size = 4096;
}