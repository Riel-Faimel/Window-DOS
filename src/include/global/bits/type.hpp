#ifndef __include_global_bits_type_hpp
#define __include_global_bits_type_hpp

namespace std {
    class strong_ordering {
        unsigned char _val;
    public:
        static constexpr unsigned char 
        less = -1,
        equal = 0,
        greater = 1;

        constexpr operator unsigned char () const {
            return _val;
        }
        constexpr strong_ordering(unsigned char v): _val{v} {}
    };
};

constexpr bool operator==(std::strong_ordering o, int zero) {
    return static_cast<int>(o) == zero;
}
constexpr bool operator!=(std::strong_ordering o, int zero) {
    return static_cast<int>(o) != zero;
}
constexpr bool operator<(std::strong_ordering o, int zero) {
    return static_cast<int>(o) < zero;
}
constexpr bool operator<=(std::strong_ordering o, int zero) {
    return static_cast<int>(o) <= zero;
}
constexpr bool operator>(std::strong_ordering o, int zero) {
    return static_cast<int>(o) > zero;
}
constexpr bool operator>=(std::strong_ordering o, int zero) {
    return static_cast<int>(o) >= zero;
}

#endif