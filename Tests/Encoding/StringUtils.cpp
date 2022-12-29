//============================================================================
// Name        : StringUtils.cpp
// Created on  : 30.01.2022
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ StringUtils src
//============================================================================

#include "StringUtils.h"

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <array>
#include <codecvt>
#include <locale>

namespace StringUtils
{
    template <typename T>
    std::string toUTF8(const std::basic_string<T, std::char_traits<T>,
    std::allocator<T>>& source)
    {
        std::string result;
        std::wstring_convert<std::codecvt_utf8_utf16<T>, T> convertor;
        result = convertor.to_bytes(source);

        return result;
    }

    template <typename T>
    void fromUTF8(const std::string& source, std::basic_string<T, std::char_traits<T>,
    std::allocator<T>>& result)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<T>, T> convertor;
        result = convertor.from_bytes(source);
    }

    std::wstring to_wchar_t(std::string str) {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{}.from_bytes(str);
    }

    // utf-8 to utf16
    std::u16string to_utf16(std::string str) {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(str);
    }

    std::u32string to_utf32(std::string str) {
        return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(str);
    }

    std::string to_utf8(std::u16string str16) {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(str16);
    }

    std::string to_utf8(std::u32string str32) {
        return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(std::u32string(str32.data(), 12));
    }

    std::string to_utf8(std::wstring wstr) {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{}.to_bytes(wstr);
    }

    std::string utf32_to_utf8(const uint32_t u32){
        if (u32<=0x7F){
            return { static_cast<char>(u32)	};								// 0XXXXXXX
        } else if (u32<=0x7FF){
            return { static_cast<char>(0xC0 | ((u32>>6) & 0x1F)),			// 110XXXXX
                     static_cast<char>(0x80 | (u32&0x3F))};				    // 10XXXXXX
        } else if (u32<=0xFFFF){
            return { static_cast<char>(0xE0 | ((u32>>12) & 0x0F)),			// 1110XXXX
                     static_cast<char>(0x80 | ((u32>>6) & 0x3F)),			// 10XXXXXX
                     static_cast<char>(0x80 | (u32&0x3F))};				    // 10XXXXXX
        } else if (u32<=0x13FFFF){
            return { static_cast<char>(0xF0 | ((u32>>18) & 0x07)),			// 11110XXX
                     static_cast<char>(0x80 | ((u32>>12) & 0x3F)),			// 10XXXXXX
                     static_cast<char>(0x80 | ((u32>>6) & 0x3F)),			// 10XXXXXX
                     static_cast<char>(0x80 | (u32&0x3F))};				    // 10XXXXXX
        } else {
            // ??? this could also be silently ignored? don't know what is better...
            throw std::invalid_argument("utf32_to_utf8: Invalid unicode codepoint.");
        }
    }

    std::string utf32_to_utf8(const std::u32string & str_u32){
        std::string str_u8;
        str_u8.reserve(str_u32.length()); // just a guess.
        for(const uint32_t u32 : str_u32)
            str_u8.append(utf32_to_utf8(u32));
        return str_u8;
    }


    std::wstring widestring(const std::string &text)
    {
        std::wstring result;
        result.resize(text.length());
        mbstowcs(&result[0], &text[0], text.length());
        return result;
    }
};

namespace StringUtils::Tests
{
    void Unicode_2_UTF8()
    {
        std::wstring uStr = L"Unicode string";
        std::string str = toUTF8(uStr);

        std::wstring after;
        fromUTF8(str, after);

        //AssertEquals(uStr, after)
        std::cout << uStr.compare(after) << std::endl;
    }

    void Utf16_2_Utf8()
    {
        std::u16string uStr;
        uStr.push_back('A');
        std::string str = toUTF8(uStr);

        std::u16string after;
        fromUTF8(str, after);

        // assert(uStr == after);
        std::cout << uStr.compare(after) << std::endl;
    }

    void Utf16_Utf32() {
        const std::string utf8 = "hello world!\n" ;

        const std::u16string utf16 = to_utf16( utf8 ) ;
        std::cout << to_utf8(utf16) ;

        const std::u32string utf32 = to_utf32( utf8 ) ;
        std::cout << to_utf8(utf32) ;
        std::cout << utf32_to_utf8(utf32) ; // INFO: Text2

        const std::wstring wstr = to_wchar_t(utf8) ;
        std::wcout << wstr ;
        std::cout << to_utf8(wstr) << "---------------------\n" ;
    }

    void Experiments() {
        const std::string utf8 = "hello world!";
        const std::u32string utf32 = to_utf32( utf8 ) ;

        std::cout << to_utf8(utf32)  << std::endl;
        std::cout << sizeof(utf32.front())  << std::endl;

       const char32_t* pts = utf32.data();
    }

    void Build_U32_String_Manualy()
    {
        // 'привет'
        std::u32string utf32 {0x43f, 0x440, 0x438, 0x432, 0x435, 0x442};

        const std::string utf8String = to_utf8(utf32);
        std::cout << utf8String << std::endl;
    }
}

template<class Facet>
struct DeletableFacet final : Facet
{
    template<class ...Args>
    explicit DeletableFacet(Args&& ...args) : Facet(std::forward<Args>(args)...) {

    }
    ~DeletableFacet() override = default;
};


void StringUtils::TestAll()
{
    // Tests::Unicode_2_UTF8();
    // Tests::Utf16_2_Utf8();

    // Tests::Utf16_Utf32();

    // Tests::Experiments();

    // Tests::Build_U32_String_Manualy();

    std::wstring_convert<DeletableFacet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> converter;
    const std::string utf8Str = "hello world!";


    std::cout << to_utf8(to_utf32(utf8Str))  << std::endl;
    std::cout << to_utf8(converter.from_bytes(utf8Str))  << std::endl;
    std::cout << to_utf8(converter.from_bytes(utf8Str))  << std::endl;
    std::cout << to_utf8(converter.from_bytes(utf8Str))  << std::endl;

};
