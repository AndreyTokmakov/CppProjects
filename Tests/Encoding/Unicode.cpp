//============================================================================
// Name        : Unicode.h
// Created on  : 01.02.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Unicode src
//============================================================================

#include "Unicode.h"

#include <iostream>
#include <string>
#include <string_view>

namespace Unicode
{

    int utf8_to_unicode(std::string_view utf8_code)
    {
        unsigned int utf8_size = utf8_code.length();
        int unicode = 0;

        for (unsigned int p = 0; p < utf8_size; ++p) {
            const int bit_count = (p? 6: 8 - utf8_size - (utf8_size == 1? 0: 1));
            const int shift = (p < utf8_size - 1? (6*(utf8_size - p - 1)): 0);
            for (int k = 0; k < bit_count; ++k)
                unicode += ((utf8_code[p] & (1 << k)) << shift);
        }
        return unicode;
    }


    std::string unicode_to_utf8(int unicode)
    {
        std::string s;

        if (unicode>=0 and unicode <= 0x7f)  { // 7F(16) = 127(10)
            s = static_cast<char>(unicode);
            return s;
        }

        else if (unicode <= 0x7ff) { // 7FF(16) = 2047(10)
            unsigned char c1 = 192, c2 = 128;
            for (int k=0; k<11; ++k) {
                if (k < 6)
                    c2 |= (unicode % 64) & (1 << k);
                else
                    c1 |= (unicode >> 6) & (1 << (k - 6));
            }
            s = c1;
            s += c2;
            return s;
        }
        else if (unicode <= 0xffff) { // FFFF(16) = 65535(10)
            unsigned char c1 = 224, c2 = 128, c3 = 128;

            for (int k=0; k<16; ++k){
                if (k < 6)  c3 |= (unicode % 64) & (1 << k);
                else if (k < 12) c2 |= (unicode >> 6) & (1 << (k - 6));
                else c1 |= (unicode >> 12) & (1 << (k - 12));
            }

            s = c1;
            s += c2;
            s += c3;

            return s;
        }
        else if (unicode <= 0x1fffff)  // 1FFFFF(16) = 2097151(10)
        {
            unsigned char c1 = 240, c2 = 128, c3 = 128, c4 = 128;
            for (int k = 0; k < 21; ++k) {
                if (k < 6)  c4 |= (unicode % 64) & (1 << k);
                else if (k < 12) c3 |= (unicode >> 6) & (1 << (k - 6));
                else if (k < 18) c2 |= (unicode >> 12) & (1 << (k - 12));
                else c1 |= (unicode >> 18) & (1 << (k - 18));
            }
            s = c1;
            s += c2;
            s += c3;
            s += c4;
            return s;
        }
        else if (unicode <= 0x3ffffff) {  // 3FFFFFF(16) = 67108863(10)
            // actually, there are no 5-bytes unicodes
        } else if (unicode <= 0x7fffffff) {  // 7FFFFFFF(16) = 2147483647(10)
            // actually, there are no 6-bytes unicodes
        } else {
            // incorrect unicode (< 0 or > 2147483647)
        }
        return {};
    }


    void Convert_Tests()
    {
        std::cout << unicode_to_utf8(36) << '\t';          // $
        std::cout << unicode_to_utf8(162) << '\t';
        std::cout << unicode_to_utf8(8364) << '\t';        // €
        std::cout << unicode_to_utf8(128578) << std::endl; // 🙂

        std::cout << unicode_to_utf8(0x24) << '\t';         // $
        std::cout << unicode_to_utf8(0xa2) << '\t';
        std::cout << unicode_to_utf8(0x20ac) << '\t';       // €
        std::cout << unicode_to_utf8(0x1f642) << std::endl; // 🙂

        std::cout << utf8_to_unicode("$") << '\t';
        std::cout << utf8_to_unicode("¢") << '\t';
        std::cout << utf8_to_unicode("€") << '\t';
        std::cout << utf8_to_unicode("🙂") << std::endl;
    }
};

constexpr char canary[] = {
        static_cast<char>(0xDE),
        static_cast<char>(0xAD),
        static_cast<char>(0xBA),
        static_cast<char>(0xBE)
};

void Unicode::TestAll()
{

    const char s[] = "\u0444";
    std::cout << s << std::endl; // 'ф'
    std::cout << "Hello, ф or \u0444!\n";




    // Unicode::Convert_Tests();

};
