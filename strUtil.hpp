#ifndef STRUTIL_HPP
#define STRUTIL_HPP

#include <iostream>
#include <vector>
#include <clocale>
#include <string>
#include <cwchar>

//需要调用std::setlocale(LC_ALL, "en_US.utf8");
std::ostream& operator<<(std::ostream &out, const wchar_t *wstr) {
    std::mbstate_t state = std::mbstate_t();
    int len = 1 + std::wcsrtombs(nullptr, &wstr, 0, &state);
    std::vector<char> mbstr(len);
    std::wcsrtombs(&mbstr[0], &wstr, mbstr.size(), &state);

    out << &mbstr[0];
    return out;
}

std::ostream& operator<<(std::ostream &out, const std::wstring &str) {
  return operator<<(out, str.c_str());
}


std::wstring str2wstr(const std::string str) {
    unsigned len = str.size() * 2;// 预留字节数
    setlocale(LC_CTYPE, "");     //必须调用此函数
    wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
    mbstowcs(p,str.c_str(),len);// 转换
    std::wstring str1(p);
    delete[] p;// 释放申请的内存
    return str1;
}

std::string wstr2str(const std::wstring str) {
    unsigned len = str.size() * 4;
    setlocale(LC_CTYPE, "");
    char *p = new char[len];
    wcstombs(p,str.c_str(),len);
    std::string str1(p);
    delete[] p;
    return str1;
}

#endif
