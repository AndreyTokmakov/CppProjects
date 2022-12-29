/**============================================================================
Name        : Base64.h
Created on  : 11.09.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Base64
============================================================================**/

#include "Base64.h"

#include <iostream>
#include <array>
#include <string>

namespace Base64 {
    static constexpr std::array<char, 64> ENCODING_TABLE{
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3',
            '4', '5', '6', '7', '8', '9', '+', '/'
    };

    static constexpr char PADDING { '=' };

    static constexpr size_t MAX_BITS_TO_ENCODE {6};
    static constexpr size_t BITS_IN_BYTE {8};

    std::string base64Encode(const std::string &str) {
        std::string result;
        result.reserve(str.size());

        int encodedByte {0};
        size_t bit_to_set {MAX_BITS_TO_ENCODE};
        for (const char byte: str) {
            for (int i = BITS_IN_BYTE - 1; i >= 0; --i, --bit_to_set) {
                if (bit_to_set <= 0) {
                    bit_to_set = MAX_BITS_TO_ENCODE;
                    // if ((result.size() + 1 ) >= result.capacity()) std::cout  << "RESIZE!" << std::endl;
                    result.append(1, ENCODING_TABLE[encodedByte]);
                    encodedByte = 0;
                }
                if (byte & (1u << i)) {
                    encodedByte |= (1 << (bit_to_set - 1));
                }
            }
        }
        result.append(1, ENCODING_TABLE[encodedByte]);
        result.append(2 == str.size() % 3 ? 1 : 2, '=');
        return result;
    }
}


namespace Tests
{
    void __test(const std::string& text, const std::string& base64Expected) {
        const std::string& actual = Base64::base64Encode(text);
        if (0 != actual.compare(base64Expected)) {
            std::cout << "Error!!! Text: " << text << ", Base64 Expected: "
                      << base64Expected << ". Actual: " << actual << std::endl;
        }
    }

    void Tests_base64Encode() {
        __test("Man", "TWFu");
        // __test("Man232321", "TWFuMjMyMzIx");
        // __test("12345", "MTIzNDU=");
        // __test("___", "X19f");
    }
};

void Base64::TestAll([[maybe_unused]] const std::vector<std::string_view>& params)
{
    // Tests::Tests_base64Encode();

    const std::string text = "dssdsdsd";
    const std::string& actual = Base64::base64Encode(text);


    std::cout << text << std::endl;
    std::cout << actual << std::endl;
};
