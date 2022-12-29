//============================================================================
// Name        : PunycodeFinal.h
// Created on  : 05.02.2022
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ TestTask src
//============================================================================

#include "PunycodeFinal.h"

#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <vector>
#include <chrono>
#include <cstring>

#include "StringUtils.h"

namespace PunycodeFinal
{
    using punycode_uint = char32_t;
    constexpr punycode_uint maxint = -1;

/* basic(cp) src whether cp is a basic code point: */
#define basic(cp) (static_cast<punycode_uint>((cp) < 0x80))

/* delim(cp) src whether cp is a delimiter: */
#define delim(cp) ((cp) == delimiter)

    enum {
        punycode_success = 0,
        punycode_overflow = -1,
        punycode_big_output = -2,
        punycode_bad_input = -3
    };

    enum {
        base = 36,
        tmin = 1,
        tmax = 26,
        skew = 38,
        damp = 700,
        initial_bias = 72,
        initial_n = 0x80,
        delimiter = 0x2D
    };

    char encode_digit(punycode_uint d, int flag) {
        return d + 22 + 75 * (d < 26) - ((flag != 0) << 5);
        /*  0..25 map to ASCII a..z or A..Z */
        /* 26..35 map to ASCII 0..9         */
    }

    unsigned int decode_digit(int cp) {
        return static_cast<unsigned int>((cp - 48 < 10 ? cp - 22 :  cp - 65 < 26 ?
                                                                    cp - 65 : cp - 97 < 26 ? cp - 97 :  base));
    }

    punycode_uint adapt(punycode_uint delta,
                        punycode_uint numpoints,
                        int firsttime)
    {
        punycode_uint k;
        delta = firsttime ? delta / damp : delta >> 1; /* delta >> 1 is a faster way of doing delta / 2 */
        delta += delta / numpoints;

        for (k = 0;  delta > ((base - tmin) * tmax) / 2;  k += base)
            delta /= base - tmin;
        return k + (base - tmin + 1) * delta / (delta + skew);
    }

    int punycode_encode(const punycode_uint input[],
                        size_t input_length_orig,
                        char* const output,
                        size_t* output_length)
    {
        /* The TestTask spec assumes that the input length is the same type */
        /* of integer as a code point, so we need to convert the size_t to  */
        /* a punycode_uint, which could overflow.                           */
        if (input_length_orig > maxint)
            return punycode_overflow;

        punycode_uint n = initial_n, delta {0}, h, b, bias = initial_bias, j, m, q, k, t;
        size_t out {0}, max_out = *output_length;
        punycode_uint input_length = static_cast<punycode_uint>(input_length_orig);

        /* Handle the basic code points: */
        for (j = 0;  j < input_length;  ++j) {
            if (basic(input[j])) {
                if (max_out - out < 2)
                    return punycode_big_output;
                output[out++] = static_cast<char>(input[j]);
            }
            /* else if (input[j] < n) return punycode_bad_input; */
            /* (not needed for TestTask with unsigned code points) */
        }

        h = b = static_cast<punycode_uint>(out);
        /* cannot overflow because out <= input_length <= maxint */

        /* h is the number of code points that have been handled, b is the  */
        /* number of basic code points, and out is the number of ASCII code */
        /* points that have been output.                                    */

        if (b > 0) output[out++] = delimiter;

        /* Main encoding loop: */
        while (h < input_length)
        {
            /* All non-basic code points < n have been     */
            /* handled already.  Find the next larger one: */

            for (m = maxint, j = 0;  j < input_length;  ++j)
            {
                /* if (basic(input[j])) continue; */
                /* (not needed for TestTask) */
                if (input[j] >= n && input[j] < m) m = input[j];
            }

            /* Increase delta enough to advance the decoder's    */
            /* <n,i> state to <m,0>, but guard against overflow: */

            if (m - n > (maxint - delta) / (h + 1)) return punycode_overflow;
            delta += (m - n) * (h + 1);
            n = m;

            for (j = 0;  j < input_length;  ++j)
            {
                /* TestTask does not need to check whether input[j] is basic: */
                if (input[j] < n /* || basic(input[j]) */ )
                {
                    if (++delta == 0) return punycode_overflow;
                }

                if (input[j] == n)
                {
                    /* Represent delta as a generalized variable-length integer: */

                    for (q = delta, k = base;  ;  k += base)
                    {
                        if (out >= max_out) return punycode_big_output;
                        t = k <= bias /* + tmin */ ? tmin :     /* +tmin not needed */
                            k >= bias + tmax ? tmax : k - bias;
                        if (q < t) break;
                        output[out++] = encode_digit(t + (q - t) % (base - t), 0);
                        q = (q - t) / (base - t);
                    }

                    output[out++] = encode_digit(q, 0);
                    bias = adapt(delta, h + 1, h == b);
                    delta = 0;
                    ++h;
                }
            }
            ++delta;
            ++n;
        }

        *output_length = out;
        return punycode_success;
    }

    int punycode_decode(const char input[],
                        size_t input_length,
                        punycode_uint output[],
                        size_t *output_length)
    {
        punycode_uint out = 0;
        punycode_uint max_out = *output_length > maxint ? maxint : static_cast<punycode_uint>(*output_length);

        /* Handle the basic code points:  Let b be the number of input code */
        /* points before the last delimiter, or 0 if there is none, then    */
        /* copy the first b code points to the output.                      */
        size_t b;
        for (size_t j = b = 0;  j < input_length;  ++j)
        {
            if (delim(input[j])) b = j;
        }
        if (b > max_out) return punycode_big_output;

        for (size_t j = 0;  j < b;  ++j)
        {
            output[out++] = input[j];
        }

        punycode_uint n = initial_n, i = 0, bias = initial_bias, oldi {0}, w, k, digit, t;

        /* Main decoding loop:  Start just after the last delimiter if any  */
        /* basic code points were copied; start at the beginning otherwise. */
        for (size_t in = b > 0 ? b + 1 : 0;  in < input_length;  ++out)
        {
            /* in is the index of the next ASCII code point to be consumed, */
            /* and out is the number of code points in the output array.    */

            /* Decode a generalized variable-length integer into delta,  */
            /* which gets added to i.  The overflow checking is easier   */
            /* if we increase i as we go, then subtract off its starting */
            /* value at the end to obtain delta.                         */

            for (oldi = i, w = 1, k = base;  ;  k += base)
            {
                if (in >= input_length) return punycode_bad_input;
                digit = decode_digit(input[in++]);
                if (digit >= base) return punycode_bad_input;
                if (digit > (maxint - i) / w) return punycode_overflow;
                i += digit * w;
                t = k <= bias /* + tmin */ ? tmin :     /* +tmin not needed */
                    k >= bias + tmax ? tmax : k - bias;
                if (digit < t) break;
                if (w > maxint / (base - t)) return punycode_overflow;
                w *= (base - t);
            }

            bias = adapt(i - oldi, out + 1, oldi == 0);

            /* i was supposed to wrap around from out+1 to 0,   */
            /* incrementing n each time, so we'll fix that now: */

            if (i / (out + 1) > maxint - n) return punycode_overflow;
            n += i / (out + 1);
            i %= (out + 1);

            /* Insert n at position i of the output: */

            /* not needed for TestTask: */
            /* if (basic(n)) return punycode_bad_input; */
            if (out >= max_out) return punycode_big_output;

            std::memmove(output + i + 1, output + i, (out - i) * sizeof *output);
            output[i++] = n;
        }

        *output_length = static_cast<size_t>(out);
        /* cannot overflow because out <= old value of *output_length */
        return punycode_success;
    }
}

namespace PunycodeFinal
{
    constexpr bool isASCII_OLD(const uint32_t u32) noexcept {
        char decoded[4] {0};
        if (u32 <= 0x7F){
            decoded[0] = static_cast<char>(u32);						    // 0XXXXXXX
        } else if (u32<=0x7FF) {
            decoded[0] = static_cast<char>(0xC0 | ((u32>>6) & 0x1F));		// 110XXXXX
            decoded[1] = static_cast<char>(0x80 | (u32&0x3F));		        // 10XXXXXX
        } else if (u32<=0xFFFF){
            decoded[0] = static_cast<char>(0xE0 | ((u32>>12) & 0x0F));		// 1110XXXX
            decoded[1] = static_cast<char>(0x80 | ((u32>>6) & 0x3F));		// 10XXXXXX
            decoded[2] = static_cast<char>(0x80 | (u32&0x3F));			    // 10XXXXXX
        } else if (u32<=0x13FFFF){
            decoded[0] = static_cast<char>(0xF0 | ((u32>>18) & 0x07));		// 11110XXX
            decoded[1] = static_cast<char>(0x80 | ((u32>>12) & 0x3F));		// 10XXXXXX
            decoded[2] = static_cast<char>(0x80 | ((u32>>6) & 0x3F));		// 10XXXXXX
            decoded[3] = static_cast<char>(0x80 | (u32&0x3F));				// 10XXXXXX
        }
        // TODO: Can it be optimized?
        return std::all_of(decoded, decoded + std::size(decoded), [](const auto c) {
            return c >= 0 && 127 >= c;
        });
    }

    //===================================================================================

    constexpr unsigned short domainMaxLength { 256 };
    constexpr unsigned short labelMaxLength { 64 };

    template<unsigned short A, unsigned short B, class ...Args>
    constexpr bool inRange(const Args& ... param) noexcept {
        return ((static_cast<char>(param) >= A && B >= static_cast<char>(param)) && ...);
    }

    template<class ...Args>
    constexpr void append(std::string& str, const Args& ... param) noexcept {
        (str.append(1, static_cast<char>(param)), ...);
    }

    constexpr bool isASCII(const uint32_t u32) noexcept {
        if (u32 <= 0x7F) {   // 0XXXXXXX
            return inRange<0, 127>(u32);
        } else if (u32 <= 0x7FF) { // 110XXXXX 10XXXXXX
            return inRange<0, 127>((0xC0 | ((u32>>6) & 0x1F)), (0x80 | (u32&0x3F)));
        } else if (u32 <= 0xFFFF) { // 1110XXXX 10XXXXXX 10XXXXXX
            return inRange<0, 127>((0xE0 | ((u32>>12) & 0x0F)), (0x80 | ((u32>>6) & 0x3F)), (0x80 | (u32&0x3F)));
        } else if (u32 <= 0x13FFFF) { // 11110XXX 10XXXXXX 10XXXXXX 10XXXXXX
            return inRange<0, 127>((0xF0 | ((u32>>18) & 0x07)), (0x80 | ((u32>>12) & 0x3F)),
                                   (0x80 | ((u32>>6) & 0x3F)), (0x80 | (u32&0x3F)));
        }
    }

    bool isASCIIStrUTF32(const std::u32string& input, size_t from, size_t until) noexcept {
        for (size_t i = from, size = std::min(input.size(), until); i < size; ++i) {
            if (!isASCII(input[i]))
                return false;
        }
        return true;
    }

    bool isEncoded(const std::string& input, size_t start) noexcept {
        return  (input.length() >= (start + 4)) &&
                (input[start] == 'x' || input[start] == 'X') &&
                (input[start + 1] == 'n' || input[start + 1] == 'N') &&
                (input[start + 2] == '-' || input[start + 4] == '-');
    };

    void appendUTF32ToString(const punycode_uint input[],
                             const size_t start, const size_t end,
                             std::string& out) noexcept {
        for (size_t i = start; i < end; ++i) {
            const uint32_t u32 = input[i];
            if (u32 <= 0x7F) {
                append(out, u32);
            } else if (u32 <= 0x7FF) {
                append(out, (0xC0 | ((u32>>6) & 0x1F)), (0x80 | (u32& 0x3F)));
            } else if (u32 <= 0xFFFF) {
                append(out, (0xE0 | ((u32>>12) & 0x0F)), (0x80 | ((u32>>6) & 0x3F)), (0x80 | (u32&0x3F)));
            } else if (u32 <= 0x13FFFF) {
                append(out, (0xF0 | ((u32>>18) & 0x07)), (0x80 | ((u32>>12) & 0x3F)),
                       (0x80 | ((u32>>6) & 0x3F)), (0x80 | (u32&0x3F)));
            }
        }
    }

    bool punycodeEncodeInternal(const std::u32string& input,
                                std::string& encodedString) noexcept
    {
        char buf[labelMaxLength] {};
        size_t prev { 0 }, length {0};
        for (size_t size = input.size(), idx = 0; idx < size; ++idx)
        {
            if ('.' == input[idx])
            {
                if (!isASCIIStrUTF32(input, prev, idx))
                {
                    const auto resultCode = punycode_encode(input.data() + prev, idx - prev,
                                                            buf, &(length = labelMaxLength));
                    if (punycode_success != resultCode)
                        return false;
                    encodedString.append("xn--").append(buf, length);
                } else
                {
                    appendUTF32ToString(input.data(), prev, idx, encodedString);
                }

                encodedString.append(1, '.');
                // To skip '.' with prefix increment '++idx'
                prev = ++idx;
            }
        }

        if (!isASCIIStrUTF32(input, prev, input.size()))
        {
            const auto resultCode = punycode_encode(input.data() + prev, input.length() - prev,
                                                    buf, &(length = labelMaxLength));
            if (punycode_success != resultCode)
                return false;
            encodedString.append("xn--").append(buf, length);
        } else
        {
            appendUTF32ToString(input.data(), prev, input.size(), encodedString);
        }
        return true;
    }

    // utf8_to_utf32
    std::string punycodeEncode(const std::string& input)
    {
        std::string::size_type start = input.find("://");
        start = std::string::npos == start ? 0 : start + 3;
        std::string::size_type last = input.find('/', start);
        last = std::string::npos == last ? input.size() : last;

        std::string encodedString {input, 0, start};
        encodedString.reserve(domainMaxLength * sizeof(punycode_uint));
        const auto& s32 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(
                (input.data() + start), (input.data() + last));
        if (!punycodeEncodeInternal(s32, encodedString))
            throw std::runtime_error("Failed to encode");
        encodedString.append(input.size() > last ? 1 : 0 , '/');
        encodedString.shrink_to_fit();
        return encodedString;
    }

    std::string punycodeDecode(const std::string& input)
    {
        std::string::size_type start = input.find("://");
        start = std::string::npos == start ? 0 : start + 3;
        std::string::size_type last = input.find('/', start);
        last = std::string::npos == last ? input.size() : last;

        std::string decodedString(input,0, start);
        decodedString.reserve(domainMaxLength);
        punycode_uint utf32Buffer[labelMaxLength] {};
        size_t prev { start }, length {0};
        for (size_t size = last, idx = start; idx < size; ++idx)
        {
            if ('.' == input[idx])
            {
                if (isEncoded(input, prev))
                {
                    prev += 4;
                    const auto resultCode = punycode_decode(input.data() + prev, idx - prev,
                                                            utf32Buffer, &(length = labelMaxLength));
                    if (punycode_success != resultCode)
                        throw std::runtime_error("Failed to decode");
                    appendUTF32ToString(utf32Buffer, 0, length, decodedString);
                } else
                {
                    decodedString.append(input, prev, idx - prev);
                }
                prev = ++idx; // Skip '.' with prefix increment
                decodedString.append(1, '.');
            }
        }

        if (isEncoded(input, prev))
        {
            prev += 4;
            const auto resultCode = punycode_decode(input.data() + prev, last - prev,
                                                    utf32Buffer, &(length = labelMaxLength));
            if (punycode_success != resultCode)
                throw std::runtime_error("Failed to decode");
            appendUTF32ToString(utf32Buffer, 0, length, decodedString);
        } else
        {
            decodedString.append(input, prev, last - prev);
        }

        decodedString.append(input.size() > last ? 1 : 0 , '/');
        decodedString.shrink_to_fit();
        return decodedString;
    }
};

namespace PunycodeFinal::Experiments
{

    std::string punycodeDecodeSLOW(const std::string& input)
    {
        std::string::size_type start = input.find("://");
        start = std::string::npos == start ? 0 : start + 3;
        std::string::size_type last = input.find('/', start);
        last = std::string::npos == last ? input.size() : last;

        std::string decodedString(input,0, start);
        punycode_uint utf32Buffer[labelMaxLength] {};
        size_t prev { start }, length {0};
        for (size_t size = last, idx = start; idx < size; ++idx)
        {
            if ('.' == input[idx])
            {
                if (isEncoded(input, prev))
                {
                    prev += 4;
                    punycode_decode(input.data() + prev, idx - prev,
                                    utf32Buffer, &(length = labelMaxLength));
                    // TODO: Check for performance
                    const std::string X = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.
                            to_bytes(utf32Buffer, utf32Buffer + length);
                    decodedString.append(X);
                }
                else
                    decodedString.append(input, prev, idx - prev);

                prev = ++idx; // Skip '.' with prefix increment
                decodedString.append(1, '.');
            }
        }

        if (isEncoded(input, prev))
        {
            prev += 4;
            punycode_decode(input.data() + prev, last - prev,
                            utf32Buffer, &(length = labelMaxLength));
            // TODO: Check for performance
            std::string X = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.
                    to_bytes(utf32Buffer, utf32Buffer + length);
            decodedString.append(X);
        } else
            decodedString.append(input, prev, last - prev);

        return decodedString;
    }
}

namespace PunycodeFinal::Tests
{
    void Tests()
    {
        const std::string input {"https://xn--b1agh1afp.yandex.xn--b1afb8abpfc.xn--80aalfgj8a.com/225555"};
        // const std::string input {"https://home.ru/"};
        std::string::size_type start = input.find("://");
        start = std::string::npos == start ? 0 : start + 3;
        std::string::size_type last = input.find('/', start);
        last = std::string::npos == last ? input.size() : last;

        std::string encodedString {input, 0, start};
        encodedString.append("_some_encoded_data_");
        encodedString.append(input.size() > last ? 1 : 0 , '/');


        std::cout  << encodedString << std::endl;
    }

    void testASCII() {
        const std::string text {"12345aaaa2222ЙЙ22"};
        const std::u32string utf32Str = StringUtils::to_utf32(text);
        for (const auto s: utf32Str)
            std::cout << std::boolalpha << isASCII(s) << "    "
                      << std::boolalpha << isASCII_OLD(s) << "\n";
    }

    void EncodeTests()
    {
        const std::string expected {"https://xn--b1agh1afp.yandex.xn--b1afb8abpfc.xn--80aalfgj8a.com22/"};
        const std::string testAddr {"https://привет.yandex.тестовое.задание.com22/"};
        const std::string encodedString = punycodeEncode(testAddr);

        std::cout << testAddr << std::endl;
        std::cout << encodedString << std::endl;
        std::cout << expected << std::endl;
    }

    void DecodeTests()
    {
        // INFO: https://xn--b1agh1afp.yandex.xn--b1afb8abpfc.xn--80aalfgj8a.com22/
        const std::string testAddr {"https://привет.yandex.тестовое.задание.com22/"};
        const std::string encodedString = punycodeEncode(testAddr);
        std::cout << testAddr << std::endl;
        // std::cout << encodedString << std::endl;
        const std::string decodedString = punycodeDecode(encodedString);
        std::cout << decodedString << std::endl;
    }

    void AllTests()
    {
        std::vector<std::pair<std::string, std::string>> values {
                {"https://привет.label.мир.com/","https://xn--b1agh1afp.label.xn--h1ahn.com/"},
                {"點看","xn--c1yn36f"},
                {"abæcdöef","xn--abcdef-qua4k"},
                {"abcdef","abcdef"},
                {"αβγ","xn--mxacd"},
                {"ยจฆฟคฏข","xn--22cdfh1b8fsa"},
                {"правда","xn--80aafi6cg"},
                {"Bahnhof München-Ost","xn--Bahnhof Mnchen-Ost-u6b"},
                {"-> $1.00 <-","-> $1.00 <-"},
                {"ドメイン名例","xn--eckwd4c7cu47r2wf"},
                {"MajiでKoiする5秒前","xn--MajiKoi5-783gue6qz075azm5e"},
                {"「bücher」","xn--bcher-kva8445foa"},
                {"https://google.com/","https://google.com/"},
        };

        for (const auto & [nameExpected, encodedExpected]:  values) {
            const std::string encodedString = punycodeEncode(nameExpected);
            if (encodedString != encodedExpected) {
                std::cout << "ERROR! Expected: " << encodedExpected << ", Actual: " << encodedString << std::endl;
                continue;
            }
            const std::string decodedString = punycodeDecode(encodedString);
            if (decodedString != nameExpected) {
                std::cout << "ERROR! Expected: " << nameExpected  << ", Actual: " << decodedString  << std::endl;
                continue;
            }
        }
    }

    void Performance()
    {
#if 1
        const std::string testString{"https://привет.yandex.тестовое.задание.com22/"};
        const std::string encodedString = punycodeEncode(testString);
        constexpr int TESTS_COUNT = 2'000'000;

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < TESTS_COUNT; ++i) {
                Experiments::punycodeDecodeSLOW(encodedString);
            }
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < TESTS_COUNT; ++i) {
                punycodeEncode(testString);
            }
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < TESTS_COUNT; ++i) {
                punycodeDecode(encodedString);
            }
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }
#endif
    }

    void Performance_ASCII()
    {
        const std::string text{"https://ввв.яндекс.рф.привет.рф.привет.рф.привет.рф.привет/"};
        const std::u32string utf32Str = StringUtils::to_utf32(text);
        constexpr int TESTS_COUNT = 20'000'000;

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < TESTS_COUNT; ++i) {
                for (const auto s: utf32Str)
                    isASCII_OLD(s);
            }

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < TESTS_COUNT; ++i) {
                for (const auto s: utf32Str)
                    isASCII(s);
            }

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }
    }
}

// TODO: Test for string_view
// TODO: Perf: input.substr(prev, 4) == "xn--"  VS 0 = 1 ... 3 == -
void PunycodeFinal::TestAll()
{
    Tests::Performance();
    // Tests::Performance_ASCII();

    // Tests::testASCII();
    // Tests::Tests();
    // Tests::EncodeTests();
    // Tests::DecodeTests();
    // Tests::AllTests();
};
