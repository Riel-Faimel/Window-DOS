#ifdef LocalDebugInfo
#undef LocalDebugInfo
#endif // clear defination before
#ifdef LocalDebug
#define LocalDebugInfo(...) __VA_ARGS__
#else
#define LocalDebugInfo(...)
#endif

#ifndef __include__global_marco_hpp
#define __include__global_marco_hpp

#ifdef _DEBUG
#define DebugInfo(...) __VA_ARGS__
#else
#define DebugInfo(...)
#endif

#define ASMLabelMark(name) asm volatile (".globl _RZAL_" #name "\n_RZAL_" #name ":");

#endif