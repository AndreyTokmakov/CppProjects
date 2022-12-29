//============================================================================
// Name        : StringUtils.h
// Created on  : 30.01.2022
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ StringUtils src
//============================================================================

#ifndef CPPCLIONPROJECTS_STRINGUTILS_H
#define CPPCLIONPROJECTS_STRINGUTILS_H

#include <string>

namespace StringUtils {
    void TestAll();

    std::u16string to_utf16(std::string str);
    std::u32string to_utf32(std::string str) ;

    std::string to_utf8(std::u16string str16);
    std::string to_utf8(std::u32string str32);
    std::string to_utf8(std::wstring wstr);
};

#endif //CPPCLIONPROJECTS_STRINGUTILS_H
