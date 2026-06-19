#include <lib/cppstdlib/string>
#include <global/type.hpp>
#define __independent_lib_Using_template_container
#include <TL/idlib>

int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    if (n == 0) return 0;
    
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;
    
    while (n-- > 0 && *p1 && (*p1 == *p2)) {
        p1++;
        p2++;
    }
    return *p1 - *p2;
}

String::String(unsigned n) noexcept {
    // turn hex to string
    rtl::array<char> hex_string(2);
    const char *hex2char = "0123456789ABCDEF";
    do {
        hex_string.append(hex2char[n & 0xF]);
        n >>= 4;
    } while(n > 0);
    data = new char[hex_string.get_size() + 1];
    for(unsigned i = 0;i < hex_string.get_size();i++){
        data[i] = hex_string[hex_string.get_size() - i - 1];
    }
    data[hex_string.get_size()] = '\0';
    len = hex_string.get_size();
}

String::String(const char *s) noexcept {
    if(s == nullptr){
        data = nullptr;
        len = 0;
    }else{
        len = 0;
        while(s[len])len++;
        data = new char[len + 1];
        for(unsigned i = 0;i < len;i++)data[i] = s[i];
        data[len] = '\0';
    }
}

String::String(char *s, unsigned n) noexcept {
    if(s == nullptr || n == 0){
        data = nullptr;
        len = 0;
    }else{
        len = n;
        data = s;
        data[n] = '\0';
    }
}

String::String(const String &s) noexcept {
    len = s.len;
    if(len){
        data = new char[len + 1];
        for(unsigned i = 0;i < len;i++)data[i] = s.data[i];
        data[len] = '\0';
    }else{
        data = nullptr;
    }
}

String::String(unsigned n, char c) noexcept :data(new char[n + 1]), len(n) {
    for(unsigned i = 0;i < n;i++)data[i] = c;
    data[n] = '\0';
}

String::~String() {
    if(data)delete[] data;
}

String& String::operator=(const String &s) noexcept {
    if(this != &s){
        if(data)delete[] data;
        len = s.len;
        if(len){
            data = new char[len + 1];
            for(unsigned i = 0;i < len;i++)data[i] = s.data[i];
            data[len] = '\0';
        }else{
            data = nullptr;
        }
    }
    return *this;
}

bool String::operator==(const String &s) const {
    if(len != s.len)return false;
    for(unsigned i = 0;i < len;i++){
        if(data[i] != s.data[i])return false;
    }
    return true;
}

bool String::operator==(const char *s) const {
    if(s == nullptr)return len == 0;
    unsigned s_len = 0;
    while(s[s_len])s_len++;
    if(len != s_len)return false;
    for(unsigned i = 0;i < len;i++){
        if(data[i] != s[i])return false;
    }
    return true;
}

String& String::operator+=(const String &s) {
    *this = *this + s;
    return *this;
}

String& String::operator+=(const char *s) {
    *this = *this + String(s);
    return *this;
}

String& String::operator+=(char ch) {
    *this = *this + ch;
    return *this;
}

String String::operator+(const String &s) const {
    String result;
    result.len = len + s.len;
    result.data = new char[result.len + 1];
    for(unsigned i = 0;i < len;i++)result.data[i] = data[i];
    for(unsigned i = 0;i < s.len;i++)result.data[len + i] = s.data[i];
    result.data[len + s.len] = '\0';
    return result;
}

String String::operator+(char ch) const {
    String result;
    result.len = len+1;
    result.data = new char[result.len + 1];
    for(unsigned i = 0;i < len;i++)result.data[i] = data[i];
    result.data[len] = ch;
    result.data[result.len] = '\0';
    return result;
}

void String::clear(){
    if(data)delete[] data;
    data = nullptr;
    len = 0;
}

void String::append(char c){
    String result(len + 1);
    for(unsigned i = 0;i < len;i++)result.data[i] = data[i];
    result.data[len] = c;
    result.data[len + 1] = '\0';
    *this = result;
}

void String::append(const char *s){
    *this = *this + String(s);
}

void String::append(const String &s){
    *this = *this + s;
}

int String::to_int() const {
    int result = 0;
    for(unsigned i = 0; i < len; i++) {
        char c = data[i];
        if(c >= '0' && c <= '9') result = result * 16 + (c - '0');
        else if(c >= 'A' && c <= 'F') result = result * 16 + (c - 'A' + 10);
        else if(c >= 'a' && c <= 'f') result = result * 16 + (c - 'a' + 10);
        else break;
    }
    return result;
}

String String::substr(int pos) const {
    if(pos < 0 || pos >= static_cast<int>(len))return String();
    return substr(pos, len - pos);
}

String String::substr(int pos, int len) const {
    if(pos < 0 || len < 0 || pos >= static_cast<int>(this->len))return String();
    if(pos + len > static_cast<int>(this->len))len = this->len - pos;
    String result(len, '\0');
    for(int i = 0;i < len;i++)result.data[i] = data[pos + i];
    result.data[len] = '\0';
    return result;
}

unsigned String::find(const String &s) const {
    for(unsigned i = 0;i <= len - s.len;i++){
        bool found = true;
        for(unsigned j = 0;j < s.len;j++){
            if(data[i + j] != s.data[j]){
                found = false;
                break;
            }
        }
        if(found)return i;
    }
    return -1;
}

unsigned String::find(const char *s) const{
    String s_str(s);
    return find(s_str);
}

String *String::split(const String &delimiter) const {
    unsigned count = 1;
    for(unsigned i = 0;i <= len - delimiter.len;i++){
        bool found = true;
        for(unsigned j = 0;j < delimiter.len;j++){
            if(data[i + j] != delimiter.data[j]){
                found = false;
                break;
            }
        }
        if(found)count++;
    }
    String *result = new String[count];
    unsigned index = 0;
    unsigned start = 0;
    for(unsigned i = 0;i <= len - delimiter.len;i++){
        bool found = true;
        for(unsigned j = 0;j < delimiter.len;j++){
            if(data[i + j] != delimiter.data[j]){
                found = false;
                break;
            }
        }
        if(found){
            result[index++] = substr(start, i - start);
            start = i + delimiter.len;
        }
    }
    result[index] = substr(start, len - start);
    return result;
}

String *String::split(const char *delimiter) const {
    String delimiter_str(delimiter);
    return split(delimiter_str);
}

String *String::split(char deli) const {
    rtl::array<String> re;
    String dealing{};
    for (auto ch : *this) {
        if (ch == deli) { re.append(dealing); }
        else { dealing+=ch; }
    }
    return re.get_ptr();
}

String String::trim() const {
    return trim_left().trim_right();
}
String String::trim_left() const {
    unsigned start = 0;
    while(start < len && (data[start] == ' ' || data[start] == '\t' || data[start] == '\n' || data[start] == '\r'))start++;
    return substr(start, len - start);
}

String String::trim_right() const {
    int end = len - 1;
    while(end >= 0 && (data[end] == ' ' || data[end] == '\t' || data[end] == '\n' || data[end] == '\r'))end--;
    return substr(0, end + 1);
}

// String.cpp
String String::extract_param(const char* param) const {
    unsigned pos = find(param);
    if(pos == (unsigned)-1) return String();
    
    pos += String(param).length();  // 跳过参数名
    
    // 跳过可能的空格
    while(pos < len && data[pos] == ' ') pos++;
    
    unsigned end = pos;
    while(end < len && data[end] != ' ') end++;
    
    return substr(pos, end - pos);
}

unsigned String::extract_int(const char* param) const {
    String val = extract_param(param);
    if(val.empty()) return -1;
    return val.to_int();
}

//=====================================================================