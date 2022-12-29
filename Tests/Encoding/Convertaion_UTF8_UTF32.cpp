//============================================================================
// Name        : Convertaion_UTF8_UTF32.h
// Created on  : 14.02.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Convertaion_UTF8_UTF32
//============================================================================

#include <iostream>
#include <memory>
#include <string>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>

#include <thread>
#include <future>

#include <mutex>
#include <atomic>

#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <array>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <span>

#include "Convertaion_UTF8_UTF32.h"
#include "StringUtils.h"

// INFO: https://gist.github.com/antonijn/9009746

namespace Convertaion_UTF8_UTF32
{
    using utf32_t = char32_t;

    bool getch(const uint8_t buf[],
               const size_t bufferSize,
               size_t& idx,
               utf32_t& len) {
        int remunits {0};
        if (idx >= bufferSize)
            return false;
        uint8_t nxt = buf[idx++], msk {0};
        if (nxt & 0x80) {
            msk = 0xe0;
            for (remunits = 1; (nxt & msk) != (msk << 1); ++remunits)
                msk = (msk >> 1) | 0x80;
        } else {
            remunits = 0;
            msk = 0;
        }
        len = nxt ^ msk;
        while (remunits-- > 0) {
            len <<= 6;
            if (idx >= bufferSize)
                return false;
            len |= buf[idx++] & 0x3f;
        }
        return true;
    }

    static int getch(const uint16_t buf[],
                     const size_t bufferSize,
                     unsigned long& idx,
                     utf32_t& cp)
    {
        if (idx >= bufferSize)
            return -1;
        uint16_t ch = buf[(idx)++];
        if ((ch & 0xfc00) != 0xd800) {
            cp = (uint32_t)ch;
            return 0;
        }
        if (idx > bufferSize)
            return -1;
        uint16_t nxt = buf[(idx)++];
        if ((nxt & 0xfc00) != 0xdc00)
            return -1;
        cp = ((ch & 0x03ff) << 10) | (nxt & 0x03ff);
        return 0;
    }

    constexpr inline int is_valid_char(const uint32_t ch) noexcept {
        return ch < 0xd800 || ch > 0xdfff;
    }

    constexpr inline int is_combo_char(const uint32_t ch) noexcept {
        return (ch >= 0x0300 && ch <= 0x036f) || (ch >= 0x20d0 && ch <= 0x20ff) || (ch >= 0xfe20 && ch <= 0xfe2f);
    }

    bool utf8_to_utf32(const uint8_t input[],
                       const size_t count,
                       utf32_t output[],
                       size_t &out_size) noexcept
    {
        for (size_t i = 0, idx = 0; i < count; ++i) {
            getch(input, count, idx, output[i]);
            if (!is_valid_char(output[i]))
                return false;
            out_size = i + 1;
        }
        return true;
    }

    int utf16_to_utf32(const uint16_t input[],
                       const size_t count,
                       utf32_t output[],
                       size_t& out_size)
    {
        for (size_t i = 0, idx = 0; i < count; ++i) {
            getch(input, count, idx, output[i]);
            if (!is_valid_char(output[i]))
                return false;
            out_size = i + 1;
        }
        return true;
    }

    void UTF8_to_UTF32_Array()
    {
        constexpr uint8_t input[] = { 'T', 'e', 's', 't'};
        std::u32string output(128, '\0');
        size_t len { output.size()};

        utf8_to_utf32(input, 4, output.data(), len);
        std::cout << "len = " << len << std::endl;

        output.erase(len);
        output.shrink_to_fit();

        std::cout << StringUtils::to_utf8(output) << std::endl;
    }

    void Char_to_UTF32_Array()
    {
        constexpr char input[] = { 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'};
        utf32_t output[128] {};
        size_t len { std::size(output)};

        std::cout << std::size(input) << std::endl;

        utf8_to_utf32(reinterpret_cast<const uint8_t *>(input), std::size(input), output, len);
        std::cout << "len = " << len << std::endl;

        std::u32string u32Str(output, len);
        u32Str.erase(len);
        u32Str.shrink_to_fit();

        std::cout << StringUtils::to_utf8(output) << std::endl;
    }

    // const std::u32string& u32Str = to_utf32(str);

    void Char_to_UTF32_Array_String()
    {
        std::string input {"qwerty"};
        utf32_t output[128] {};
        size_t len { std::size(output)};

        utf8_to_utf32(reinterpret_cast<const uint8_t *>(input.data()), input.size(), output, len);
        std::cout << "len = " << len << std::endl;

        std::u32string u32Str(output, len);
        u32Str.erase(len);
        u32Str.shrink_to_fit();

        std::cout << StringUtils::to_utf8(output) << std::endl;
    }

    void UTF16_to_UTF32_Array()
    {
        constexpr uint16_t input[] = { 0x444, 0x438, 0x438};
        utf32_t output[128] {};
        size_t len { std::size(output)};


        utf16_to_utf32(input, 3, output, len);
        std::cout << "len = " << len << std::endl;

        std::u32string u32Str(output, len);
        u32Str.erase(len);
        u32Str.shrink_to_fit();

        std::cout << StringUtils::to_utf8(u32Str) << std::endl;
    }
};

template<class Facet>
struct deletable_facet : Facet
{
    template<class ...Args>
    deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

void Convertaion_UTF8_UTF32::TestAll()
{

    // UTF8_to_UTF32_Array();
    // Char_to_UTF32_Array();
    // Conv::Char_to_UTF32_Array_String();
    // Conv::UTF16_to_UTF32_Array();


    /*
    std::wstring_convert<deletable_facet<std::codecvt<char32_t, char8_t, std::mbstate_t>>, char32_t> convToUtf8;

    const std::string text {"qwerty_12345"};
    std::u32string str32 = StringUtils::to_utf32(text);

    auto x = convToUtf8.to_bytes(str32);

    //std::cout << convToUtf8.to_bytes(str32) << std::endl;
    std::cout << StringUtils::to_utf8(str32) << std::endl;
    */
};
