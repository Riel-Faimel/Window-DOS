#ifndef __lib_cppstdlib_string_hpp
#define __lib_cppstdlib_string_hpp
class String{
    char *data;
    unsigned int len;
public:
    String();
    String(const char *s) noexcept;
    String(char *s, unsigned n) noexcept;
    String(const String &s) noexcept;
    String(String &&s) noexcept;
    String(unsigned n) noexcept;
    String(unsigned n, char c) noexcept;
    ~String();

    String& operator=(const String &s) noexcept;
    String& operator=(String &&s) noexcept;

    int length() const;
    bool empty() const;
    const char * c_str() const;

    bool operator==(const String &s) const;
    bool operator==(const char *s) const;
    bool operator!=(const String &s) const;

    String operator+(const String &s) const;
    String operator+(const char *s) const;
    String& operator+=(const String &s);
    String& operator+=(const char *s);

    void clear();
    void append(char c);
    void append(const char *s);
    void append(const String &s);

    int to_int() const;
    String substr(int pos) const;
    String substr(int pos, int len) const;

    int find(const String &s) const;
    int find(const char *s) const;

    String *split(const String &delimiter) const;
    String *split(const char *delimiter) const;

    String trim() const;
    String trim_left() const;
    String trim_right() const;

    String extract_param(const char *) const;
    int extract_int(const char *) const;

    operator const char *() const;
};

inline String::String():data(nullptr), len(0) {}
inline String::String(String &&s) noexcept :data(s.data), len(s.len){
    s.data = nullptr;
    s.len = 0;
}
inline String& String::operator=(String &&s) noexcept {
    if(this != &s){
        if(data)delete[] data;
        data = s.data;
        len = s.len;
        s.data = nullptr;
        s.len = 0;
    }
    return *this;
}
inline bool String::operator!=(const String &s) const {
    return !(*this == s);
}
inline String::operator const char *() const {
    return c_str();
}
inline int String::length() const {
    return len;
}
inline const char * String::c_str() const {
    return data ? data : "";
}
inline bool String::empty() const {
    return len == 0;
}
#endif