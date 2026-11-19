/*
 * Minimal Itanium C++ ABI RTTI runtime for renew-DOS.
 *
 * This file supplies only what `dynamic_cast` needs:
 *   - the __class_type_info / __si_class_type_info / __vmi_class_type_info
 *     object layout that -frtti translation units emit,
 *   - the vtables those emitted typeinfo objects point at,
 *   - __dynamic_cast().
 *
 * scope: single and multiple *non-virtual* inheritance.
 * out of scope: virtual bases, typeid()/.name(), exception type matching.
 * A cast that would need a virtual base simply reports "not found".
 */

namespace __cxxabiv1 {

using rtti_ptrdiff = __PTRDIFF_TYPE__;

struct __find_result {
    int count;
    rtti_ptrdiff offset;
};

class __class_type_info {
public:
    const char *__name;

    __class_type_info(const char *name = nullptr) : __name(name) {}

    virtual ~__class_type_info();
    virtual void __collect(const __class_type_info *target,
                           rtti_ptrdiff offset,
                           __find_result &result) const;
};

class __si_class_type_info : public __class_type_info {
public:
    const __class_type_info *__base_type;

    ~__si_class_type_info() override;
    void __collect(const __class_type_info *target,
                   rtti_ptrdiff offset,
                   __find_result &result) const override;
};

struct __base_class_type_info {
    const __class_type_info *__base_type;
    long __offset_flags;

    bool __is_virtual_p() const { return (__offset_flags & 0x1) != 0; }
    rtti_ptrdiff __offset() const { return __offset_flags >> 8; }
};

class __vmi_class_type_info : public __class_type_info {
public:
    unsigned int __flags;
    unsigned int __base_count;
    __base_class_type_info __base_info[1];

    ~__vmi_class_type_info() override;
    void __collect(const __class_type_info *target,
                   rtti_ptrdiff offset,
                   __find_result &result) const override;
};

/* Builtin and pointer typeinfo: real objects for the _ZTIi / _ZTIPKc family
 * so throw/catch of fundamental and pointer types can match by name. */

class __fundamental_type_info : public __class_type_info {
public:
    explicit __fundamental_type_info(const char *name) : __class_type_info(name) {}
    ~__fundamental_type_info() override;
};

class __pbase_type_info : public __class_type_info {
public:
    unsigned int __flags;
    const __class_type_info *__pointee;

    __pbase_type_info(const char *name, unsigned int flags,
                      const __class_type_info *pointee)
        : __class_type_info(name), __flags(flags), __pointee(pointee) {}

    ~__pbase_type_info() override;
};

class __pointer_type_info : public __pbase_type_info {
public:
    __pointer_type_info(const char *name, unsigned int flags,
                        const __class_type_info *pointee)
        : __pbase_type_info(name, flags, pointee) {}

    ~__pointer_type_info() override;
};

/* out-of-line dtors double as key functions: these are the only translation
 * units that emit the three typeinfo vtables. */
__class_type_info::~__class_type_info() {}
__si_class_type_info::~__si_class_type_info() {}
__vmi_class_type_info::~__vmi_class_type_info() {}
__fundamental_type_info::~__fundamental_type_info() {}
__pbase_type_info::~__pbase_type_info() {}
__pointer_type_info::~__pointer_type_info() {}

static bool name_equal(const char *a, const char *b) {
    if (a == b) return true;
    if (!a || !b) return false;
    while (*a && *a == *b) { ++a; ++b; }
    return *a == *b;
}

/* typeinfo identity: pointer equality first, then name equality.  Separate
 * translation units may hold separate copies of the same _ZTI symbol when
 * they are not merged by the linker, so the name fallback is required. */
static bool same_type(const __class_type_info *a, const __class_type_info *b) {
    return a == b || (a && b && name_equal(a->__name, b->__name));
}

void __class_type_info::__collect(const __class_type_info *target,
                                  rtti_ptrdiff offset,
                                  __find_result &result) const {
    if (same_type(this, target)) {
        if (result.count == 0) result.offset = offset;
        ++result.count;
    }
}

void __si_class_type_info::__collect(const __class_type_info *target,
                                     rtti_ptrdiff offset,
                                     __find_result &result) const {
    if (same_type(this, target)) {
        if (result.count == 0) result.offset = offset;
        ++result.count;
        return;
    }
    __base_type->__collect(target, offset, result);
}

void __vmi_class_type_info::__collect(const __class_type_info *target,
                                      rtti_ptrdiff offset,
                                      __find_result &result) const {
    if (same_type(this, target)) {
        if (result.count == 0) result.offset = offset;
        ++result.count;
        return;
    }
    for (unsigned int i = 0; i < __base_count; ++i) {
        const __base_class_type_info &base = __base_info[i];
        if (base.__is_virtual_p()) continue;
        base.__base_type->__collect(target, offset + base.__offset(), result);
    }
}

} // namespace __cxxabiv1

extern "C" void *__dynamic_cast(const void *sub,
                                const __cxxabiv1::__class_type_info *src,
                                const __cxxabiv1::__class_type_info *dst,
                                __cxxabiv1::rtti_ptrdiff src2dst) {
    (void)src2dst;

    if (!sub || !src) return nullptr;

    /* The complete object's typeinfo lives in the vtable's -1 slot.  It is
     * zero for objects compiled with -fno-rtti, which makes the cast fail
     * instead of reading garbage. */
    void **vptr = *(void ***)sub;
    if (!vptr) return nullptr;
    const __cxxabiv1::__class_type_info *dynamic =
        (const __cxxabiv1::__class_type_info *)vptr[-1];
    if (!dynamic) return nullptr;

    __cxxabiv1::__find_result src_find{0, 0};
    dynamic->__collect(src, 0, src_find);
    if (src_find.count != 1) return nullptr;

    char *complete = (char *)sub - src_find.offset;

    /* dynamic_cast<void*> asks for the most-derived object. */
    if (!dst) return complete;

    __cxxabiv1::__find_result dst_find{0, 0};
    dynamic->__collect(dst, 0, dst_find);
    if (dst_find.count != 1) return nullptr;

    return complete + dst_find.offset;
}

/* The ABI vtables emitted below; declared as byte arrays so the check can
 * compare vptr values without knowing their element type. */
extern "C" {
extern char _ZTVN10__cxxabiv117__class_type_infoE[];
extern char _ZTVN10__cxxabiv120__si_class_type_infoE[];
extern char _ZTVN10__cxxabiv121__vmi_class_type_infoE[];
extern char _ZTVN10__cxxabiv123__fundamental_type_infoE[];
extern char _ZTVN10__cxxabiv117__pbase_type_infoE[];
extern char _ZTVN10__cxxabiv119__pointer_type_infoE[];
}

/* Real typeinfo from the compiler has one of the vtables above.  The dummy
 * _ZTIi/_ZTIPKc labels used to fail this check; they are real objects now. */
static bool is_known_typeinfo(const void *ti) {
    const void *vptr = *(const void * const *)ti;
    return vptr == (const void *)(_ZTVN10__cxxabiv117__class_type_infoE + 8)
        || vptr == (const void *)(_ZTVN10__cxxabiv120__si_class_type_infoE + 8)
        || vptr == (const void *)(_ZTVN10__cxxabiv121__vmi_class_type_infoE + 8)
        || vptr == (const void *)(_ZTVN10__cxxabiv123__fundamental_type_infoE + 8)
        || vptr == (const void *)(_ZTVN10__cxxabiv117__pbase_type_infoE + 8)
        || vptr == (const void *)(_ZTVN10__cxxabiv119__pointer_type_infoE + 8);
}

/* LSDA catch matching: is `catch_type` a unique base of the thrown type?
 * Returns the byte offset of the catch subobject inside the thrown object.
 * A null catch type is catch(...). */
extern "C" bool __eh_type_match(const void *thrown_ti, const void *catch_ti,
                                int *offset) {
    if (!catch_ti) { *offset = 0; return true; }
    if (!thrown_ti) return false;
    if (!is_known_typeinfo(thrown_ti)) return false;
    if (!is_known_typeinfo(catch_ti)) return false;

    const auto *thrown = (const __cxxabiv1::__class_type_info *)thrown_ti;
    const auto *cat = (const __cxxabiv1::__class_type_info *)catch_ti;

    __cxxabiv1::__find_result result{0, 0};
    thrown->__collect(cat, 0, result);
    if (result.count != 1) return false;

    *offset = (int)result.offset;
    return true;
}

/* __cxa_bad_cast / __cxa_bad_typeid live in eh_cxa_exp.cpp: they throw
 * std::bad_cast / std::bad_typeid so a normal catch can receive them. */

/* GCC already emits weak _ZTIi / _ZTIPKc / ... for fundamental and pointer
 * types in this TU, so no hand-written stubs are needed. */
