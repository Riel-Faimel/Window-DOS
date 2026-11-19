#include <stdio>

namespace {

constexpr char hex_digits[] = "0123456789ABCDEF";

size_t hex_into(char *out, u32 v, int digits, bool prefix) {
    size_t n = 0;
    if (prefix) {
        out[n++] = '0';
        out[n++] = 'x';
    }
    for (int shift = (digits - 1) * 4; shift >= 0; shift -= 4) {
        out[n++] = hex_digits[(v >> shift) & 0x0Fu];
    }
    return n;
}

size_t str_len(const char *s) {
    size_t n = 0;
    while (s[n]) ++n;
    return n;
}

} // namespace

ostream &ostream::operator<< (const char *s) {
    if (s) emit(s, str_len(s));
    return *this;
}

ostream &ostream::operator<< (char c) {
    emit(&c, 1);
    return *this;
}

ostream &ostream::operator<< (bool b) {
    const char *s = b ? "true" : "false";
    emit(s, str_len(s));
    return *this;
}

ostream &ostream::operator<< (unsigned char i) {
    char buf[10];
    emit(buf, hex_into(buf, i, 2, true));
    return *this;
}

ostream &ostream::operator<< (unsigned short i) {
    char buf[10];
    emit(buf, hex_into(buf, i, 4, true));
    return *this;
}

ostream &ostream::operator<< (unsigned i) {
    char buf[10];
    emit(buf, hex_into(buf, i, 8, true));
    return *this;
}

ostream &ostream::operator<< (int i) {
    return *this << static_cast<unsigned>(i);
}

ostream &ostream::operator<< (unsigned long long i) {
    return *this << static_cast<unsigned>(i);
}

ostream &ostream::operator<< (u64 i) {
    char buf[10];
    emit(buf, hex_into(buf, i.high, 8, true));
    emit(buf, hex_into(buf, i.low, 8, false));
    return *this;
}

ostream &ostream::operator<< (void *p) {
    char buf[10];
    emit(buf, hex_into(buf, reinterpret_cast<u32>(p), 8, true));
    return *this;
}

void ostream::ch(char c) {
    emit(&c, 1);
}

void ostream::str(const char *s, char end) {
    if (!s) return;

    size_t n = 0;
    while (s[n] && s[n] != end) ++n;
    emit(s, n);
}

void ostream::hex(unsigned char i) {
    char buf[10];
    emit(buf, hex_into(buf, i, 2, false));
}

void ostream::hex(unsigned short i) {
    char buf[10];
    emit(buf, hex_into(buf, i, 4, false));
}

void ostream::hex(unsigned i) {
    char buf[10];
    emit(buf, hex_into(buf, i, 8, false));
}

void ostream::hex(int i) {
    hex(static_cast<unsigned>(i));
}
