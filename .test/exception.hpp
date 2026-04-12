// exception_abi_stub.hpp
// 纯ABI存根实现 - 只提供符号，不做任何异常处理

#ifndef EXCEPTION_ABI_STUB_HPP
#define EXCEPTION_ABI_STUB_HPP

// 最基本的类型定义
#define _BITS_64
#include "../src/include/global/type.hpp"

// 最小化的type_info定义
extern "C" {

namespace std {
    class type_info {
    public:
        virtual ~type_info() = default;
    };
    // 为常用类型提供typeinfo占位符
    type_info _ZTIPKc;  // const char*
    type_info _ZTIi;     // int
    type_info _ZTId;     // double
}

}

// unwind基本类型
enum _Unwind_Reason_Code {
    _URC_NO_REASON = 0,
    _URC_FOREIGN_EXCEPTION = 4,
    _URC_CONTINUE_UNWIND = 8,
    _URC_END_OF_STACK = 5
};

enum _Unwind_Action {
    _UA_SEARCH_PHASE = 1,
    _UA_CLEANUP_PHASE = 2,
    _UA_HANDLER_FRAME = 4
};

struct _Unwind_Exception {
    unsigned long long exception_class;
    void (*exception_cleanup)(_Unwind_Reason_Code, _Unwind_Exception*);
    unsigned long long private_1;
    unsigned long long private_2;
};

struct _Unwind_Context {
    // 空结构
};

// 简单内存函数声明
//extern "C" void* malloc(size_t size);
extern "C" void free(void* ptr);
extern "C" void abort(void) __attribute__((noreturn));

extern "C" void abort(){
    while(1);
}

extern "C" int atexit(void (*)()){
    return 0;
}

void operator delete (void *, unsigned long long ){}
// ============ ABI符号存根实现 ============

extern "C" {

// 1. 核心异常函数
void* __cxa_allocate_exception(unsigned long long thrown_size) throw() {
    return nullptr;  // 不做分配
}

void __cxa_free_exception(void* thrown_exception) noexcept {
    // 空实现
}

void __cxa_throw(
    void* thrown_exception, 
    void *tinfo,
    void (*destructor)(void*)
){
    abort();  // 直接终止
}

void* __cxa_begin_catch(void* exceptionObject) noexcept {
    return exceptionObject;  // 原样返回
}

void __cxa_end_catch(){
    // 空实现
}

void __cxa_rethrow() noexcept {
    abort();  // 直接终止
}

void *__cxa_current_exception_type() noexcept {
    return nullptr;
}

// 2. 纯虚函数相关
void __cxa_pure_virtual() {
    abort();
}

void __cxa_deleted_virtual() {
    abort();
}

// 3. 保护函数
void __cxa_guard_acquire() {}
void __cxa_guard_release() {}
void __cxa_guard_abort() {}

// 4. 动态初始化
void __cxa_atexit() {}
void __cxa_finalize() {}

// 5. unwind函数
_Unwind_Reason_Code _Unwind_RaiseException(_Unwind_Exception* obj) noexcept {
    return _URC_END_OF_STACK;
}

_Unwind_Reason_Code _Unwind_Resume(_Unwind_Exception* obj) noexcept {
    return _URC_CONTINUE_UNWIND;
}

void _Unwind_DeleteException(_Unwind_Exception* obj) noexcept {
    // 空实现
}

_Unwind_Reason_Code _Unwind_Resume_or_Rethrow(_Unwind_Exception* obj) noexcept {
    return _URC_CONTINUE_UNWIND;
}

unsigned long long _Unwind_GetIP(_Unwind_Context* ctx) noexcept {
    return 0;
}

void _Unwind_SetIP(_Unwind_Context* ctx, unsigned long long ip) noexcept {
    // 空实现
}

unsigned long long _Unwind_GetCFA(_Unwind_Context* ctx) noexcept {
    return 0;
}

unsigned long long _Unwind_GetGR(_Unwind_Context* ctx, int reg) noexcept {
    return 0;
}

void _Unwind_SetGR(_Unwind_Context* ctx, int reg, unsigned long long val) noexcept {
    // 空实现
}

unsigned long long _Unwind_GetLanguageSpecificData(_Unwind_Context* ctx) noexcept {
    return 0;
}

unsigned long long _Unwind_GetRegionStart(_Unwind_Context* ctx) noexcept {
    return 0;
}

// 6. personality函数
_Unwind_Reason_Code __gxx_personality_v0(
    int version,
    _Unwind_Action actions,
    unsigned long long exceptionClass,
    _Unwind_Exception* unwind_exception,
    _Unwind_Context* context
) noexcept {
    return _URC_CONTINUE_UNWIND;
}

// Windows SEH版本
_Unwind_Reason_Code __gxx_personality_seh0(
    int version,
    _Unwind_Action actions,
    unsigned long long exceptionClass,
    _Unwind_Exception* unwind_exception,
    _Unwind_Context* context
) noexcept {
    return _URC_CONTINUE_UNWIND;
}

// SJLJ版本
_Unwind_Reason_Code __gxx_personality_sj0(
    int version,
    _Unwind_Action actions,
    unsigned long long exceptionClass,
    _Unwind_Exception* unwind_exception,
    _Unwind_Context* context
) noexcept {
    return _URC_CONTINUE_UNWIND;
}

// 7. MinGW特定符号
void __main() {
    // 空实现
}

} // extern "C"


namespace __cxxabiv1 {
    class __class_type_info : public std::type_info {
    public:
        __class_type_info() : std::type_info() {}
        virtual ~__class_type_info() {}
        
        // 最少一个虚函数实现
        virtual bool __do_catch(const std::type_info*, void**, unsigned) const {
            return false;
        }
    };

    class __si_class_type_info : public std::type_info{
        public:
        __si_class_type_info() : std::type_info(){}
        virtual bool exam(){
            return true;
        }
    };
    
    // 这会强制生成虚表
    __class_type_info __class_type_info_instance;
    __si_class_type_info __si_class_type_info_instance;
}

#endif // EXCEPTION_ABI_STUB_HPP