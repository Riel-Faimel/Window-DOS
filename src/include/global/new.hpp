#ifndef __include_global_new_hpp
#define __include_global_new_hpp

#include <global/type.hpp>

void *operator new(size_t);
void *operator new(size_t, void *);
void operator delete(void *);
void operator delete(void *, unsigned);
void operator delete[](void *);
void operator delete[](void *, unsigned);

#endif