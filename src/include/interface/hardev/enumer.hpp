#ifndef __include_interface_enumer_hpp
#define __include_interface_enumer_hpp

#include <global/type.hpp>

class Enumerator {
public:
    class EnumerIndex {
        u16 id = 0;
    public:
        EnumerIndex() = default;
        constexpr explicit EnumerIndex(u16 id_): id{id_} {}
        constexpr explicit operator u16 () const { return id; }

        bool operator == (const EnumerIndex &oth) const { return id == oth.id; }
        EnumerIndex &operator ++ () { id++; return *this; }
        EnumerIndex operator ++ (int) { return EnumerIndex{id++}; }
    } id{};
    virtual ~Enumerator() = default;
    virtual void probe() = 0;
};

#endif