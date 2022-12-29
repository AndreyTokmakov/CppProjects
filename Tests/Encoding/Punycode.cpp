//============================================================================
// Name        : TestTask.cpp
// Created on  : 30.01.2022
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ TestTask src
//============================================================================

#include "Punycode.h"

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <codecvt>
#include <locale>
#include <chrono>
#include <cstring>
#include <cassert>


#include "StringUtils.h"

namespace Punycode
{

    /* punycode parameters, see http://tools.ietf.org/html/rfc3492#section-5 */
#define BASE 36
#define TMIN 1
#define TMAX 26
#define SKEW 38
#define DAMP 700
#define INITIAL_N 128
#define INITIAL_BIAS 72

#define basic(cp) ((punycode_uint)(cp) < 0x80)

    using punycode_uint = char32_t;
    constexpr punycode_uint maxint = -1;

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

    punycode_uint adapt(punycode_uint delta,
                        punycode_uint numpoints,
                        int firsttime)
    {
        punycode_uint k;
        delta = firsttime ? delta / damp : delta >> 1;
        /* delta >> 1 is a faster way of doing delta / 2 */
        delta += delta / numpoints;

        for (k = 0;  delta > ((base - tmin) * tmax) / 2;  k += base) {
            delta /= base - tmin;
        }

        return k + (base - tmin + 1) * delta / (delta + skew);
    }

    uint32_t adapt_bias(uint32_t delta,
                        unsigned int n_points,
                        int is_first) {
        uint32_t k;
        delta /= is_first ? DAMP : 2;
        delta += delta / n_points;

        /* while delta > 455: delta /= 35 */
        for (k = 0; delta > ((BASE - TMIN) * TMAX) / 2; k += BASE) {
            delta /= (BASE - TMIN);
        }

        return k + (((BASE - TMIN + 1) * delta) / (delta + SKEW));
    }

    char encode_digit(int c) {
        assert(c >= 0 && c <= BASE - TMIN);
        if (c > 25) {
            return c + 22; /* '0'..'9' */
        }
        else {
            return c + 'a'; /* 'a'..'z' */
        }
    }

    /* Encode as a generalized variable-length integer. Returns number of bytes written. */
    static size_t encode_var_int(const size_t bias, const size_t delta, char* const dst, size_t dstlen) {
        size_t i, k, q, t;

        i = 0;
        k = BASE;
        q = delta;

        while (i < dstlen) {
            if (k <= bias) {
                t = TMIN;
            }
            else if (k >= bias + TMAX) {
                t = TMAX;
            }
            else {
                t = k - bias;
            }

            if (q < t) {
                break;
            }

            dst[i++] = encode_digit(t + (q - t) % (BASE - t));

            q = (q - t) / (BASE - t);
            k += BASE;
        }

        if (i < dstlen) {
            dst[i++] = encode_digit(q);
        }

        return i;
    }

    static size_t decode_digit(uint32_t v) {
        if (isdigit(v)) {
            return 22 + (v - '0');
        }
        if (islower(v)) {
            return v - 'a';
        }
        if (isupper(v)) {
            return v - 'A';
        }
        return SIZE_MAX;
    }

    size_t punycode_encode(const uint32_t* const src,
                           const size_t srclen,
                           char* const dst,
                           size_t* const dstlen) {
        size_t b, h;
        size_t delta, bias;
        size_t m, n;
        size_t si, di;

        for (si = 0, di = 0; si < srclen && di < *dstlen; si++) {
            if (src[si] < 128) {
                dst[di++] = src[si];
            }
        }

        b = h = di;

        /* Write out delimiter if any basic code points were processed. */
        if (di > 0 && di < *dstlen) {
            dst[di++] = '-';
        }

        n = INITIAL_N;
        bias = INITIAL_BIAS;
        delta = 0;

        for (; h < srclen && di < *dstlen; n++, delta++) {
            /* Find next smallest non-basic code point. */
            for (m = SIZE_MAX, si = 0; si < srclen; si++) {
                if (src[si] >= n && src[si] < m) {
                    m = src[si];
                }
            }

            if ((m - n) > (SIZE_MAX - delta) / (h + 1)) {
                /* OVERFLOW */
                assert(0 && "OVERFLOW");
                goto fail;
            }

            delta += (m - n) * (h + 1);
            n = m;

            for (si = 0; si < srclen; si++) {
                if (src[si] < n) {
                    if (++delta == 0) {
                        /* OVERFLOW */
                        assert(0 && "OVERFLOW");
                        goto fail;
                    }
                }
                else if (src[si] == n) {
                    di += encode_var_int(bias, delta, &dst[di], *dstlen - di);
                    bias = adapt_bias(delta, h + 1, h == b);
                    delta = 0;
                    h++;
                }
            }
        }

        fail:
        /* Tell the caller how many bytes were written to the output buffer. */
        *dstlen = di;

        /* Return how many Unicode code points were converted. */
        return si;
    }

    size_t punycode_decode(const char* const src,
                           const size_t srclen,
                           uint32_t* const dst,
                           size_t* const dstlen) {
        const char* p;
        size_t b, n, t;
        size_t i, k, w;
        size_t si, di;
        size_t digit;
        size_t org_i;
        size_t bias;

        /* Ensure that the input contains only ASCII characters. */
        for (si = 0; si < srclen; si++) {
            if (src[si] & 0x80) {
                *dstlen = 0;
                return 0;
            }
        }

        /* Reverse-search for delimiter in input. */
        for (p = src + srclen - 1; p > src && *p != '-'; p--) { }
        b = p - src;

        /* Copy basic code points to output. */
        di = std::min(b, *dstlen);

        for (i = 0; i < di; i++) {
            dst[i] = src[i];
        }

        i = 0;
        n = INITIAL_N;
        bias = INITIAL_BIAS;

        for (si = b + (b > 0); si < srclen && di < *dstlen; di++) {
            org_i = i;

            for (w = 1, k = BASE; di < *dstlen; k += BASE) {
                digit = decode_digit(src[si++]);

                if (digit == SIZE_MAX) {
                    goto fail;
                }

                if (digit > (SIZE_MAX - i) / w) {
                    /* OVERFLOW */
                    assert(0 && "OVERFLOW");
                    goto fail;
                }

                i += digit * w;

                if (k <= bias) {
                    t = TMIN;
                }
                else if (k >= bias + TMAX) {
                    t = TMAX;
                }
                else {
                    t = k - bias;
                }

                if (digit < t) {
                    break;
                }

                if (w > SIZE_MAX / (BASE - t)) {
                    /* OVERFLOW */
                    assert(0 && "OVERFLOW");
                    goto fail;
                }

                w *= BASE - t;
            }

            bias = adapt_bias(i - org_i, di + 1, org_i == 0);

            if (i / (di + 1) > SIZE_MAX - n) {
                /* OVERFLOW */
                assert(0 && "OVERFLOW");
                goto fail;
            }

            n += i / (di + 1);
            i %= (di + 1);

            memmove(dst + i + 1, dst + i, (di - i) * sizeof(uint32_t));
            dst[i++] = n;
        }

        fail:
        /* Tell the caller how many bytes were written to the output buffer. */
        *dstlen = di;

        return si;
    }

    //----------------------------------------------------------------

    size_t punycode_encode_ex(const char32_t* const src,
                              const size_t srclen,
                              char* const dst,
                              size_t& dstlen) {
        size_t si, di;
        for (si = 0, di = 0; si < srclen && di < dstlen; ++si) {
            if (src[si] < 128) {
                dst[di++] = src[si];
            }
        }

        /* Write out delimiter if any basic code points were processed. */
        if (di > 0 && di < dstlen) {
            dst[di++] = '-';
        }

        size_t n = INITIAL_N, bias = INITIAL_BIAS, delta = 0, b = di, m;
        for (size_t h = di; h < srclen && di < dstlen; ++n, ++delta) {
            /* Find next smallest non-basic code point. */
            for (m = SIZE_MAX, si = 0; si < srclen; si++) {
                if (src[si] >= n && src[si] < m) {
                    m = src[si];
                }
            }

            if ((m - n) > (SIZE_MAX - delta) / (h + 1)) { /* OVERFLOW */
                dstlen = di;
                return si;
            }

            delta += (m - n) * (h + 1);
            n = m;

            for (si = 0; si < srclen; ++si) {
                if (src[si] < n) {
                    if (++delta == 0) { /* OVERFLOW */
                        dstlen = di;
                        return si;
                    }
                }
                else if (src[si] == n) {
                    di += encode_var_int(bias, delta, &dst[di], dstlen - di);
                    bias = adapt_bias(delta, h + 1, h == b);
                    delta = 0;
                    h++;
                }
            }
        }

        dstlen = di;
        return si;
    }

    //=====================================================================================

    void Assert(bool condition) {
        if (!condition) {
            std::cout << "Failed!!\n";
        }
    }


    constexpr char canary[] = {
            static_cast<char>(0xDE),
            static_cast<char>(0xAD),
            static_cast<char>(0xBA),
            static_cast<char>(0xBE)
    };

    size_t unilen(const uint32_t* input) {
        const uint32_t* p;
        for (p = input; *p; p++) {}
        return p - input;
    }

    size_t unilen_ex(const char32_t* input) {
        const char32_t* p;
        for (p = input; *p; p++) {}
        return p - input;
    }

    void write_canary(void* dst, size_t len) {
        for (size_t i = 0; i < len; i += sizeof(canary)) {
            memcpy((char*)dst + i, canary, std::min(len - i, sizeof canary));
        }
    }

    void check_canary([[maybe_unused]] const void* dst, size_t len, size_t off) {
        for (size_t i = off; i < len; ++i) {
            assert(*((const char*)dst + off) == canary[off % sizeof(canary)]);
        }
    }

    void test_encoder(const uint32_t* input,
                      const char* expected) {
        char buffer[1024];
        size_t n_converted {0}, length = sizeof(buffer);

        // std::cout << "unilen(input) = " << unilen(input) << std::endl;

        write_canary(buffer, length);
        n_converted = punycode_encode(input, unilen(input), buffer, &length);

        // std::cout << "length = " << length << std::endl;

        Assert(length <= sizeof buffer);
        check_canary(buffer, sizeof buffer, length);

        Assert(n_converted == unilen(input));
        Assert(memcmp(buffer, expected, strlen(expected)) == 0);
    }

    void test_decoder(const char* input, const uint32_t* expected) {
        uint32_t dstbuf[1024];
        size_t n_converted;
        size_t dstlen = sizeof(dstbuf);

        write_canary(dstbuf, dstlen);
        n_converted = punycode_decode(input, strlen(input), dstbuf, &dstlen);

        Assert(dstlen <= sizeof(dstbuf));
        check_canary(dstbuf, sizeof dstbuf, dstlen * sizeof(dstbuf[0]));

        Assert(n_converted == strlen(input));
        Assert(memcmp(dstbuf, expected, unilen(expected)) == 0);
    }

    struct TestData {
        const uint32_t* unicode;
        const char* punycode;
    };

    void Test1() {
        constexpr uint32_t stanza0[] = { 0xFC, 0 };

        // Bach (only basic code points)
        constexpr uint32_t stanza1[] = { 'B', 'a', 'c', 'h', 0 };

        // bücher
        constexpr uint32_t stanza2[] = { 'b', 0xFC, 'c', 'h', 'e', 'r', 0 };

        // Willst du die Blüthe des frühen, die Früchte des späteren Jahres
        constexpr uint32_t stanza3[] = {
                'W', 'i', 'l', 'l', 's', 't', ' ', 'd', 'u', ' ',
                'd', 'i', 'e', ' ', 'B', 'l', 0xFC, 't', 'h', 'e', ' ',
                'd', 'e', 's', ' ', 'f', 'r', 0xFC, 'h', 'e', 'n', ',', ' ',
                'd', 'i', 'e', ' ', 'F', 'r', 0xFC, 'c', 'h', 't', 'e', ' ',
                'd', 'e', 's', ' ', 's', 'p', 0xE4, 't', 'e', 'r', 'e', 'n', ' ',
                'J', 'a', 'h', 'r', 'e', 's', 0
        };

        // frühen
        constexpr uint32_t stanza4[] = {
                'f', 'r', 0xFC, 'h', 'e', 'n', 0
        };

        // Blüthe
        [[maybe_unused]] constexpr uint32_t stanza5[] = {
                'B', 'l', 0xFC, 't', 'h', 'e', 0
        };

        std::array<TestData, 5> simple_tests {{
                                                      { stanza0, "tda" }, // ü --> tda
                                                      { stanza1, "Bach-" },
                                                      { stanza2, "bcher-kva" },
                                                      { stanza3, "Willst du die Blthe des frhen, die Frchte des spteren Jahres-x9e96lkal" },
                                                      { stanza4, "frhen-lva" },
                                                      // { stanza5, "Blthe-lva" },
                                              }};

        for (const auto& test: simple_tests)
            test_encoder(test.unicode, test.punycode);

        /*
        for (const auto& test : simple_tests)
            test_decoder(test.punycode, test.unicode);
        */
    }

    void DecodeTest(const std::u32string& src,
                    const std::string& expected)
    {
        char buffer[1024] {0};
        size_t length = sizeof(buffer);

        // write_canary(buffer, length);
        const size_t len = punycode_encode_ex(src.data(), unilen_ex(src.data()), buffer, length);

        // std::cout << len << "  " << length << std::endl;

        Assert(length <= sizeof(buffer));
        // check_canary(buffer, sizeof(buffer), length);
        Assert(len == unilen_ex(src.data()));

        const std::string result(buffer, length);
        Assert(expected == buffer);

        std::cout << "Actual: " << result << "   Expected: " << expected << std::endl;
        // std::cout << StringUtils::to_utf8(tests) << " --> " << std::string(buffer, length) << std::endl;
    }
}

namespace ClickHouse
{
    using punycode_uint = char32_t;
    constexpr punycode_uint maxint = -1;

/* basic(cp) src whether cp is a basic code point: */
#define basic(cp) ((punycode_uint)(cp) < 0x80)

/* delim(cp) src whether cp is a delimiter: */
#define delim(cp) ((cp) == delimiter)

    //================================================================================


    template<class Facet>
    struct DeletableFacet final : Facet
    {
        template<class ...Args>
        explicit DeletableFacet(Args&& ...args) : Facet(std::forward<Args>(args)...) {

        }
        ~DeletableFacet() override = default;
    };

    // std::wstring_convert<DeletableFacet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> converter;
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter {};



    //================================================================================

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

    unsigned decode_digit(int cp) {
        return (unsigned) (cp - 48 < 10 ? cp - 22 :  cp - 65 < 26 ? cp - 65 : cp - 97 < 26 ? cp - 97 :  base);
    }

    size_t unilen_ex(const char32_t* input) {
        const char32_t* p;
        for (p = input; *p; p++) {}
        return p - input;
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

    void Assert(bool condition) {
        if (!condition) {
            std::cout << "Failed!!\n";
        }
    }


    //-----------------------------------------------------------------------

    int punycode_encode(const punycode_uint input[],
                        size_t input_length_orig,
                        char* const output,
                        size_t* output_length)
    {
        punycode_uint input_length, n, delta, h, b, bias, j, m, q, k, t;
        size_t out, max_out;

        /* The TestTask spec assumes that the input length is the same type */
        /* of integer as a code point, so we need to convert the size_t to  */
        /* a punycode_uint, which could overflow.                           */
        if (input_length_orig > maxint)
            return punycode_overflow;
        input_length = (punycode_uint) input_length_orig;

        /* Initialize the state: */
        n = initial_n;
        delta = 0;
        out = 0;
        max_out = *output_length;
        bias = INITIAL_BIAS;

        /* Handle the basic code points: */
        for (j = 0;  j < input_length;  ++j) {
            if (basic(input[j])) {
                if (max_out - out < 2)
                    return punycode_big_output;
                output[out++] = (char) input[j];
            }
            /* else if (input[j] < n) return punycode_bad_input; */
            /* (not needed for TestTask with unsigned code points) */
        }

        h = b = (punycode_uint) out;
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
            ++delta, ++n;
        }

        *output_length = out;
        return punycode_success;
    }

    int punycode_decode(size_t input_length,
                        const char input[],
                        size_t *output_length,
                        punycode_uint output[])
    {
        punycode_uint n, out, i, max_out, bias, oldi, w, k, digit, t;
        size_t b, j, in;

        /* Initialize the state: */

        n = initial_n;
        out = i = 0;
        max_out = *output_length > maxint ? maxint : (punycode_uint) *output_length;
        bias = initial_bias;

        /* Handle the basic code points:  Let b be the number of input code */
        /* points before the last delimiter, or 0 if there is none, then    */
        /* copy the first b code points to the output.                      */

        for (b = j = 0;  j < input_length;  ++j)
        {
            if (delim(input[j])) b = j;
        }
        if (b > max_out) return punycode_big_output;

        for (j = 0;  j < b;  ++j)
        {
            if (!basic(input[j])) return punycode_bad_input;
            output[out++] = input[j];
        }

        /* Main decoding loop:  Start just after the last delimiter if any  */
        /* basic code points were copied; start at the beginning otherwise. */

        for (in = b > 0 ? b + 1 : 0;  in < input_length;  ++out)
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

        *output_length = (size_t) out;
        /* cannot overflow because out <= old value of *output_length */
        return punycode_success;
    }

    bool isEncodedIDN(const std::string& hostname)
    {
        return hostname.compare(0, 4, "xn--") == 0 ||
               hostname.find(".xn--") != std::string::npos;
    }

    //---------------------------------------------------------------------------------------------

    constexpr unsigned short IDN2_DOMAIN_MAX_LENGTH = 255;
    constexpr unsigned short IDN2_LABEL_MAX_LENGTH = 63;

    using utf32_t = char32_t;

    constexpr inline int is_valid_char(const uint32_t ch) noexcept {
        return ch < 0xd800 || ch > 0xdfff;
    }

    constexpr inline int is_combo_char(const uint32_t ch) noexcept {
        return (ch >= 0x0300 && ch <= 0x036f) || (ch >= 0x20d0 && ch <= 0x20ff) || (ch >= 0xfe20 && ch <= 0xfe2f);
    }

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

    static int getch16(const uint16_t buf[],
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
        for (unsigned long i = 0, idx = 0; i < count; ++i) {
            getch16(input, count, idx, output[i]);
            if (!is_valid_char(output[i]))
                return false;
            out_size = i + 1;
        }
        return true;
    }

    punycode_uint * idn2_to_unicode_8z4z(const char *input,
                                         size_t &len)
    {
        punycode_uint domain_u32[256] {};
        punycode_uint out_u32[IDN2_DOMAIN_MAX_LENGTH + 1] {};
        size_t out_len = 0;
        const char *e, *s;

        for (e = s = input; *e; s = e) {
            punycode_uint label_u32[IDN2_LABEL_MAX_LENGTH];
            size_t label_len = IDN2_LABEL_MAX_LENGTH;

            while (*e && *e != '.')
                e++;

            std::cout << "e: " << e << " | s: " << s << " | Dif: " << e -s  << std::endl;
            if (e - s >= 4 && (s[0] == 'x' || s[0] == 'X') && (s[1] == 'n' || s[1] == 'N') && s[2] == '-' && s[3] == '-')
            {
                std::cout << "Before punycode_decode()\n";
                s += 4;
                if (int rc = punycode_decode(e - s, (char*)s, &label_len, label_u32); rc)
                    return nullptr;
                if (out_len + label_len + (*e == '.') > IDN2_DOMAIN_MAX_LENGTH)
                    return nullptr;
                memcpy (out_u32 + out_len, label_u32, label_len);

            } else {
                /* convert UTF-8 input to UTF-32 */
                utf8_to_utf32((uint8_t *) s, e - s, domain_u32, label_len);

                if (label_len > IDN2_LABEL_MAX_LENGTH) {
                    std::cout << "IDN2_LABEL_MAX_LENGTH\n";
                    return nullptr;
                }
                if (out_len + label_len + (*e == '.') > IDN2_DOMAIN_MAX_LENGTH) {
                    std::cout << "IDN2_DOMAIN_MAX_LENGTH\n";
                    return nullptr;
                }
                memcpy(out_u32 + out_len, domain_u32, label_len * sizeof(punycode_uint));

                {
                    std::u32string out(out_u32, out_len + label_len);
                    std::cout << "Test encoded [" << label_len << "]: " << StringUtils::to_utf8(out) << std::endl;
                    std::cout << "out_len = " << out_len << std::endl << std::endl;
                }
            }

            out_len += label_len;
            if (*e) {
                out_u32[out_len++] = '.';
                e++;
            }
        }

        /*
        if (output) {
            out_u32[out_len] = 0;
            auto *_out = new punycode_uint[out_len + 1];
            memcpy(out_u32 + out_len, domain_u32, (out_len + 1) * sizeof(punycode_uint));
            len = out_len;
            if (!_out)
                return false;
            output = _out;
            std::cout << "----\n";
        }
        */

        punycode_uint* output = new punycode_uint[out_len + 1];
        output[out_len + 1] = '\0';
        memcpy(output, out_u32, (out_len + 1) * sizeof(punycode_uint));
        len = out_len;

        {
            std::u32string out(output, out_len);
            std::cout << "At the end: [" << StringUtils::to_utf8(out) << "]"<< std::endl;
        }
        return output;
    }

    //---------------------------------------------------------------------------------------------

    void DecodeTestBuf(const std::u32string& src,
                       const std::string& expected)
    {
        char buffer[1024] {0};
        size_t length = sizeof(buffer);

        [[maybe_unused]]
        const size_t len = punycode_encode(src.data(), unilen_ex(src.data()),
                                           buffer, &length);

        // std::cout << len << "  " << length << std::endl;

        // Assert(length <= sizeof(buffer));
        // Assert(len == unilen_ex(tests.data()));
        const std::string result(buffer, length);
        Assert(expected == buffer);

        std::cout << "Actual: " << result << "   Expected: " << expected << std::endl;
        // std::cout << StringUtils::to_utf8(tests) << " --> " << std::string(buffer, length) << std::endl;
    }

    std::wstring to_wchar_t(const std::string& str) {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{}.from_bytes(str);
    }

    // utf-8 to utf16
    std::u16string to_utf16(const std::string& str) {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(str);
    }

    std::u32string to_utf32(const std::string& str) {
        return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(str);
    }

    std::u32string to_utf32_View(std::string_view str) {
        return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(str.data());
    }

    std::string to_utf8(const std::u16string& str16) {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(str16);
    }

    std::string to_utf8(const std::u32string& str32) {
        return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(str32);
    }

    std::string to_utf8(const std::wstring& wstr) {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{}.to_bytes(wstr);
    }

    void DecodeTest(const std::u32string& src,
                    const std::string& expected)
    {
        std::string decodedString(1024, '\0');
        size_t length = decodedString.size();
        [[maybe_unused]]
        const int result = punycode_encode(src.data(), src.size(),
                                           decodedString.data(), &length);
        decodedString.erase(length);
        decodedString.shrink_to_fit();
        std::cout << "Actual: " << decodedString << "   Expected: " << expected << std::endl;
    }

    void EncodeTest(const std::u32string& src,
                    const std::string& expected)
    {
        std::u32string decodedString(256, '\0');
        size_t length = decodedString.size();
        [[maybe_unused]]
        size_t n_converted = punycode_decode(expected.size(),
                                             expected.data(), &length, decodedString.data());
        decodedString.erase(length);
        decodedString.shrink_to_fit();
        std::cout << "Actual: " << to_utf8(decodedString) << "   Expected: " << to_utf8(src) << std::endl;
    }

    void encodeUtf32Str(const std::u32string& input) {
        char buf[64] {};
        size_t length {0};
        punycode_encode(input.data(), input.size(), buf, &(length = 64));

        std::string encodedString {"xn--"};
        encodedString.append(buf, length);

        std::cout << encodedString << std::endl;
    }

    // [[nodiscard("Bla-bla")]]
    std::string punycodeEncode(const std::u32string& input) {
        std::string encodedString {};
        encodedString.reserve(1024);
        char buf[64] {};

        size_t prev { 0 }, decodedLength {0}, length {0};
        for (size_t size = input.size(), idx = 0; idx < size; ++idx) {
            if ('.' == input[idx]) {
                punycode_encode(input.data() + prev, idx - prev, buf, &(length = 64)); // FIXME: 128
                // TODO: Check result
                encodedString.append("xn--").append(buf, length).append(".");
                decodedLength += length + 5;

                // Skip '.' with prefix increment
                prev = ++idx;
            }
        }

        punycode_encode(input.data() + prev, input.length() - prev, buf, &(length = 64)); // FIXME: 128
        // TODO: Check result
        encodedString.append("xn--").append(buf, length); // FIXME: to constant
        decodedLength += length + 4;

        encodedString.erase(decodedLength);
        encodedString.shrink_to_fit();
        return encodedString;
    }

    std::string punycodeEncode1(std::string_view input)
    {
        std::string encodedString {};
        encodedString.reserve(1024);
        char buf[64] {};

        std::string str;
        size_t prev { 0 }, decodedLength {0}, length {8};
        for (size_t size = input.size(), idx = 0; idx < size; ++idx) {
            if ('.' == input[idx])
            {
                // TODO: std::string_view??
                // TODO: Convert part of std::string to u32string without creating std::string object
                str.assign(input.data() + prev, idx - prev);
                // const std::string_view view = input.substr(prev, idx - prev);
                // std::cout << str.size() << " " << view.size() << " " << idx - prev << std::endl;
                if (std::any_of(str.cbegin(), str.cend(), [](auto c) {
                    return !(c >= 0 && 127 >=c);
                })) {
                    // const std::u32string u32Str = to_utf32_View(str);
                    const std::u32string& u32Str = to_utf32(str);
                    // std::cout << to_utf8(u32Str)  << std::endl;
                    //std::cout << "NON ANSI: [" << to_utf8(u32Str) << "]" << std::endl;
                    punycode_encode(u32Str.data(), u32Str.size(), buf, &(length = 64)); // FIXME: 128
                    // TODO: Check result
                    encodedString.append("xn--").append(buf, length);
                    decodedLength += length + 5;
                    //std::cout << "Encoded: [xn--" << std::string(buf, length) << "]" << std::endl;
                }
                else {
                    encodedString.append(input.data() + prev, idx - prev);
                    decodedLength += str.size() + 1;
                    //std::cout << "ANSI: [" << str << "]" << std::endl;
                    //std::cout << "Encoded: [" << std::string() << "]" << std::endl;
                }

                // Skip '.' with prefix increment
                prev = ++idx;
                encodedString.append(".");
            }
        }

        // TODO: std::string_view??
        str.assign(input.data() + prev, input.length() - prev);
        if (std::any_of(str.cbegin(), str.cend(), [](auto c) {
            return !(c >= 0 && 127 >=c);
        })) {
            const std::u32string& u32Str = to_utf32_View(str);
            //std::cout << "NON ANSI: [" << to_utf8(u32Str) << "]" << std::endl;
            punycode_encode(u32Str.data(), u32Str.size(), buf, &(length = 64)); // FIXME: 128
            // TODO: Check result
            encodedString.append("xn--").append(buf, length); // FIXME: to constant
            decodedLength += length + 4;
            //std::cout << "Encoded: [xn--" << std::string(buf, length) << "]" << std::endl;
        } else {
            encodedString.append(str);
            decodedLength += str.size();
            // std::cout << "ANSI: [" << str << "]" << std::endl;
            // std::cout << "Encoded: [" << str << "]" << std::endl;
        }

        encodedString.erase(decodedLength);
        encodedString.shrink_to_fit();
        return encodedString;
    }

    //================================================================================

    constexpr bool isASCII(const uint32_t u32) noexcept {
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
        return std::all_of(decoded, decoded + std::size(decoded), [](const auto c) {
            return c >= 0 && 127 >= c;
        });
    }

    size_t fromUTF32(const std::u32string& input,
                     size_t from, size_t until,
                     char decoded[]) noexcept {
        size_t len = 0;
        for (size_t i = from, size = std::min(input.size(), until); i < size; ++i) {
            const uint32_t u32 = input[i];
            if (u32 <= 0x7F) {
                decoded[len++] = static_cast<char>(u32);                         // 0XXXXXXX
            } else if (u32 <= 0x7FF) {
                decoded[len++] = static_cast<char>(0xC0 | ((u32 >> 6) & 0x1F));  // 110XXXXX
                decoded[len++] = static_cast<char>(0x80 | (u32 & 0x3F));         // 10XXXXXX
            } else if (u32 <= 0xFFFF) {
                decoded[len++] = static_cast<char>(0xE0 | ((u32 >> 12) & 0x0F)); // 1110XXXX
                decoded[len++] = static_cast<char>(0x80 | ((u32 >> 6) & 0x3F));  // 10XXXXXX
                decoded[len++] = static_cast<char>(0x80 | (u32 & 0x3F));         // 10XXXXXX
            } else if (u32 <= 0x13FFFF) {
                decoded[len++] = static_cast<char>(0xF0 | ((u32 >> 18) & 0x07)); // 11110XXX
                decoded[len++] = static_cast<char>(0x80 | ((u32 >> 12) & 0x3F)); // 10XXXXXX
                decoded[len++] = static_cast<char>(0x80 | ((u32 >> 6) & 0x3F));  // 10XXXXXX
                decoded[len++] = static_cast<char>(0x80 | (u32 & 0x3F));         // 10XXXXXX
            }
        }
        return len;
    }

    size_t fromUTF32_FAST(const std::u32string& input,
                          size_t from, size_t until,
                          std::string& out) noexcept {
        size_t len = 0;
        for (size_t i = from, size = std::min(input.size(), until); i < size; ++i) {
            const uint32_t u32 = input[i];
            if (u32 <= 0x7F) {
                len += 1;
                out.append(1, static_cast<char>(u32));                            // 0XXXXXXX
            } else if (u32 <= 0x7FF) {
                len += 2;
                out.append(1, static_cast<char>(0xC0 | ((u32 >> 6) & 0x1F)));  // 110XXXXX
                out.append(1, static_cast<char>(0x80 | (u32 & 0x3F)));         // 10XXXXXX
            } else if (u32 <= 0xFFFF) {
                len += 3;
                out.append(1, static_cast<char>(0xE0 | ((u32 >> 12) & 0x0F))); // 1110XXXX
                out.append(1, static_cast<char>(0x80 | ((u32 >> 6) & 0x3F)));  // 10XXXXXX
                out.append(1, static_cast<char>(0x80 | (u32 & 0x3F)));         // 10XXXXXX
            } else if (u32 <= 0x13FFFF) {
                len += 4;
                out.append(1, static_cast<char>(0xF0 | ((u32 >> 18) & 0x07))); // 11110XXX
                out.append(1, static_cast<char>(0x80 | ((u32 >> 12) & 0x3F))); // 10XXXXXX
                out.append(1, static_cast<char>(0x80 | ((u32 >> 6) & 0x3F)));  // 10XXXXXX
                out.append(1, static_cast<char>(0x80 | (u32 & 0x3F)));         // 10XXXXXX
            }
        }
        return len;
    }

    constexpr bool isASCII(const char symbol) noexcept {
        return 0 <= symbol && symbol <= 127;
    }

    bool isASCIIStrUTF8(const std::string& input, size_t from, size_t until) noexcept {
        for (size_t i = from, size = std::min(input.size(), until); i < size; ++i) {
            if (!isASCII(input[i]))
                return false;
        }
        return true;
    }

    bool isASCIIStrUTF32(const std::u32string& input, size_t from, size_t until) noexcept {
        for (size_t i = from, size = std::min(input.size(), until); i < size; ++i) {
            if (!isASCII(input[i]))
                return false;
        }
        return true;
    }

    std::string punycodeEncodeFinal(const std::u32string& input) {
        std::string encodedString {};
        encodedString.reserve(1024);
        char buf[64] {};

        size_t prev { 0 }, decodedLength {0}, length {0};
        for (size_t size = input.size(), idx = 0; idx < size; ++idx) {
            if ('.' == input[idx])
            {
                if (!isASCIIStrUTF32(input, prev, idx)) {
                    punycode_encode(input.data() + prev, idx - prev, buf, &(length = 64)); // FIXME: 128
                    // TODO: Check result
                    encodedString.append("xn--").append(buf, length).append(".");
                    decodedLength += length + 5;
                } else {
                    length = fromUTF32(input, prev, idx, buf);
                    encodedString.append(buf, length).append(".");
                    decodedLength += length + 1;
                }
                prev = ++idx; // Skip '.' with prefix increment
            }
        }

        // TODO: Check result
        if (!isASCIIStrUTF32(input, prev, input.size())) {
            punycode_encode(input.data() + prev, input.length() - prev, buf, &(length = 64));
            // TODO: Check response
            encodedString.append("xn--").append(buf, length);
            decodedLength += length + 4;
        } else {
            length = fromUTF32(input, prev, input.size(), buf);
            encodedString.append(buf, length);
            decodedLength += length;
        }

        encodedString.erase(decodedLength);
        encodedString.shrink_to_fit();
        return encodedString;
    }

    constexpr std::string_view FILLER = "\"xn--";

    size_t punycodeEncodeFinalOut(const std::u32string& input,
                                  std::string& encodedString) {
        char buf[64] {};
        size_t prev { 0 }, length {0};
        for (size_t size = input.size(), idx = 0; idx < size; ++idx) {
            if ('.' == input[idx])
            {
                if (!isASCIIStrUTF32(input, prev, idx)) {
                    punycode_encode(input.data() + prev, idx - prev,
                                    buf, &(length = sizeof(buf)));
                    // TODO: Check response
                    encodedString.append("xn--");
                } else {
                    length = fromUTF32(input, prev, idx, buf);
                }
                encodedString.append(buf, length).append(1, '.');
                prev = ++idx; // Skip '.' with prefix increment
            }
        }

        if (!isASCIIStrUTF32(input, prev, input.size())) {
            punycode_encode(input.data() + prev, input.length() - prev,
                            buf, &(length = sizeof(buf)));
            // TODO: Check response
            encodedString.append("xn--");
        } else {
            length = fromUTF32(input, prev, input.size(), buf);
        }
        encodedString.append(buf, length);
        return encodedString.length();
    }

    void  punycodeEncodeFinalOut_FAST(const std::u32string& input,
                                      std::string& encodedString) {
        size_t prev { 0 }, decodedLength {encodedString.length()}, length {0};
        for (size_t size = input.size(), idx = 0; idx < size; ++idx) {
            if ('.' == input[idx])
            {
                if (!isASCIIStrUTF32(input, prev, idx)) {
                    encodedString.append("xn--");
                    encodedString.append(64, '\0');
                    punycode_encode(input.data() + prev, idx - prev,
                                    encodedString.data() + decodedLength + 4, &(length = 64));
                    // TODO: Check result
                    decodedLength += length + 4;
                    encodedString.erase(decodedLength);
                    encodedString.append(".");
                    decodedLength++;

                } else {
                    fromUTF32_FAST(input, prev, idx, encodedString);
                    decodedLength = encodedString.append(".").length();
                }
                prev = ++idx; // Skip '.' with prefix increment
            }
        }

        if (!isASCIIStrUTF32(input, prev, input.size())) {
            encodedString.append("xn--");
            encodedString.append(64, '\0');
            punycode_encode(input.data() + prev, input.length() - prev,
                            encodedString.data() + decodedLength + 4, &(length = 64));
            // TODO: Check response
            decodedLength += length + 4;
            encodedString.erase(decodedLength);
        } else {
            fromUTF32_FAST(input, prev, input.size(), encodedString);
        }
    }


    std::string punycodeEncodeFinalUTF8(const std::string& input)
    {
        std::string::size_type start = input.find("://");
        start = std::string::npos == start ? 0 : start + 3;
        std::string::size_type last = input.find('/', start);
        last = std::string::npos == last ? input.size() : last;

        std::string encodedString {input, 0, start}, s8;
        encodedString.reserve(1024);
        char buf[64] {};
        size_t prev { start }, decodedLength {start}, length {0};
        for (size_t idx = start; idx < last; ++idx)
        {
            if ('.' == input[idx])
            {
                s8.assign(input, prev, idx - prev);
                if (!isASCIIStrUTF8(input, prev, idx)) {
                    auto s = to_utf32(s8);
                    punycode_encode(s.data(), s.size(), buf, &(length = 64)); // FIXME: 128
                    // std::cout << "xn--" << std::string(buf, length) << ".";
                    encodedString.append("xn--").append(buf, length).append(1, '.');
                    decodedLength += length + 5;
                }
                else {
                    // std::cout << s8 << std::endl;
                    encodedString.append(input, prev, idx - prev).append(1, '.');
                    decodedLength += idx - prev + 1;
                }
                prev = ++idx; // Skip '.' with prefix increment
            }
        }

        s8.assign(input, prev, last - prev);
        if (!isASCIIStrUTF8(input, prev, last)) { // FIXME: signature missmatch
            auto s = to_utf32(s8);
            punycode_encode(s.data(), s.size(), buf, &(length = 64)); // FIXME: 128
            // std::cout << "xn--" << std::string(buf, length) << std::endl;
            encodedString.append("xn--").append(buf, length);
            decodedLength += length + 4;
        }
        else {
            // std::cout << s8 << std::endl;
            encodedString.append(input, prev, last - prev);
            decodedLength += last - prev;
        }

        encodedString.erase(decodedLength);
        encodedString.shrink_to_fit();
        return encodedString;
    }

    // utf8_to_utf32
    std::string punycodeEncodeFinalUTF8_EX(const std::string& input)
    {
        std::string::size_type start = input.find("://");
        start = std::string::npos == start ? 0 : start + 3;
        std::string::size_type last = input.find('/', start);
        last = std::string::npos == last ? input.size() : last;

        std::string encodedString {input, 0, start};
        encodedString.reserve(1024);
        const auto& s32 = converter.from_bytes((input.data() + start), (input.data() + last));
        //const auto& s32 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes((input.data() + start), (input.data() + last));
        const size_t len = punycodeEncodeFinalOut(s32, encodedString);
        encodedString.erase(len);
        encodedString.shrink_to_fit();
        return encodedString;
    }

    std::string punycodeEncodeFinalUTF8_EX_FAST(const std::string& input)
    {
        std::string::size_type start = input.find("://");
        start = std::string::npos == start ? 0 : start + 3;
        std::string::size_type last = input.find('/', start);
        last = std::string::npos == last ? input.size() : last;

        std::string encodedString {input, 0, start};
        encodedString.reserve(1024);
        const auto& s32 = converter.from_bytes((input.data() + start), (input.data() + last));
        // const auto& s32 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes((input.data() + start), (input.data() + last));
        punycodeEncodeFinalOut_FAST(s32, encodedString);
        encodedString.shrink_to_fit();
        return encodedString;
    }

    //=======================================================================================================

    void punycodeEncodeDebug(const std::u32string& input) {
        char buf[64] {};

        size_t prev { 0 }, length {0};
        for (size_t size = input.size(), idx = 0; idx < size; ++idx) {
            if ('.' == input[idx]) {
                std::cout << "xn--";

                const std::u32string src(input, prev, idx - prev);
                punycode_encode(src.data(), src.size(), buf, &(length = 64)); // FIXME: 128
                std::cout << std::string(buf, length) << ".";

                // Skip '.' with prefix increment
                prev = ++idx;
            }
        }

        std::cout << "xn--";
        const std::u32string src(input, prev, input.length() - prev);
        punycode_encode(src.data(), src.size(), buf, &(length = 64)); // FIXME: 128
        std::cout << std::string(buf, length) << std::endl;
    }
};

namespace Punycode::Decoding
{
    void punycodeDecode(const std::u32string& src,
                        const std::string& expected)
    {
        std::string decodedString(1024, '\0');
        size_t length = decodedString.size();
        [[maybe_unused]]
        const int result = ClickHouse::punycode_encode(src.data(), src.size(), decodedString.data(), &length);
        decodedString.erase(length);
        decodedString.shrink_to_fit();
        std::cout << "Actual: " << decodedString << "   Expected: " << expected << std::endl;
    }

    void splitTest(const std::string& input)
    {
        std::string::size_type start = input.find("://");
        start = std::string::npos == start ? 0 : start + 3;
        std::string::size_type last = input.find('/', start);
        last = std::string::npos == last ? input.size() : last;


        // char buf[64] {};
        size_t prev { start }, length {0};
        for (size_t size = last, idx = start; idx < size; ++idx) {
            if ('.' == input[idx])
            {
                std::cout << std::string(input, prev, idx - prev) << std::endl;
                prev = ++idx; // Skip '.' with prefix increment
            }
        }

        std::cout << std::string(input, prev, last - prev) << std::endl;
    }
}

// INFO:  ü  --> 000000fc -->  0xFC

void Punycode::TestAll()
{
    // Test1();

    std::vector<std::pair<std::u32string, std::string>> values;
    // ü --> tda
    values.emplace_back(std::u32string{0xFC}, "tda");
    values.emplace_back(std::u32string{0x43f, 0x440, 0x438, 0x432, 0x435, 0x442}, "b1agh1afp");
    // правда  --> b1agh1afp
    values.emplace_back(std::u32string{0x43f, 0x440, 0x430, 0x0432, 0x434, 0x430}, "80aafi6cg");
    // ยจฆฟคฏข --> 22cdfh1b8fsa
    values.emplace_back(std::u32string{0xe22, 0xe08, 0xe06, 0xe1f, 0xe04, 0xe0f, 0xe02}, "22cdfh1b8fsa");
    // хостингрф --> c1ajpcnchub
    values.emplace_back(std::u32string{0x445, 0x43e, 0x441, 0x442, 0x438, 0x43d, 0x433, 0x440, 0x444},
                        "c1ajpcnchub");
    // München-Ost --> Mnchen-Ost-9db
    values.emplace_back(std::u32string{0x4d, 0xfc, 0x06e, 0x63, 0x68, 0x65, 0x6e, 0x2d, 0x4f, 0x73, 0x74},
                        "Mnchen-Ost-9db");
    values.emplace_back(std::u32string {0x42 ,0x61 ,0x68 ,0x6e ,0x68 ,0x6f ,0x66 ,0x20 ,0x4d ,
                                        0xfc, 0x6e ,0x63 ,0x68 ,0x65 ,0x6e ,0x2d ,0x4f ,0x73 ,0x74}, "Bahnhof Mnchen-Ost-u6b");


    // ClickHouse::EncodeTest(values[1].first, values[1].second);
    // ClickHouse::DecodeTest(values[1].first, values[1].second);
    // Decoding::punycodeDecode(values[1].first, values[1].second);


    const std::string testAddr {"https://привет.yandex.тестовое.задание.com22/"};
    const std::string encodedString = ClickHouse::punycodeEncodeFinalUTF8_EX(testAddr);

    std::cout << encodedString << std::endl;


    /*
    size_t len = 0;
    punycode_uint *result = ClickHouse::idn2_to_unicode_8z4z ("supertest.ru.com", len);

    std::u32string output(result, len);
    std::cout << "X:" << StringUtils::to_utf8(output) << std::endl;
    */

    /*
    for (const auto& [char64String, expected]: values) {
        // DecodeTest(char64String, expected);
        // ClickHouse::DecodeTestBuf(char64String, expected);
        // ClickHouse::DecodeTest(char64String, expected);
        ClickHouse::EncodeTest(char64String, expected);
    }
    */



    std::u32string name {0x434, 0x43e, 0x43c, 0x02e, 0x440, 0x444};

    // какой-то.длинный.домен
    // xn----7sb1acbyc5a.xn--d1aidgla2h.xn--d1acufc
    std::u32string name1 {0x43a ,0x430 ,0x43a ,0x43e ,0x439 ,0x02d ,0x442 ,0x43e ,
                          0x02e ,0x434 ,0x43b ,0x438 ,0x43d ,0x43d ,0x44b ,0x439 ,
                          0x02e ,0x434 ,0x43e ,0x43c ,0x435 ,0x43d};

    // какой-то.длинный.домен.comZZ
    // xn----7sb1acbyc5a.xn--d1aidgla2h.xn--d1acufc.xn--com-
    // xn----7sb1acbyc5a.xn--d1aidgla2h.xn--d1acufc.com
    const std::u32string name2 {0x43a, 0x430, 0x43a ,0x43e ,0x439 ,0x02d ,0x442 ,0x43e ,
                                0x02e, 0x434, 0x43b ,0x438 ,0x43d ,0x43d ,0x44b ,0x439 ,
                                0x02e, 0x434, 0x43e ,0x43c ,0x435 ,0x43d ,0x02e ,0x063 ,0x06f ,0x06d, 0x05a, 0x05a};
    const std::string u8StrName2 { "какой-то.длинный.домен.comZZ" };

    // std::cout << ClickHouse::to_utf8(name2) << std::endl;
    // std::cout << u8StrName2<< std::endl;



    /*
    char outBuf[128] {0};
    const std::u32string input = ClickHouse::to_utf32("abc");
    size_t len = ClickHouse::fromUTF32(input, 0, input.size(), outBuf);
    std::cout << len << std::endl;
    std::cout << std::string(outBuf, len) << std::endl;
    */


#if 0
    const std::u32string input = ClickHouse::to_utf32("https://ввв.яндекс.рф/");
    // const std::string result = ClickHouse::punycodeEncodeFinal(input);
    // const std::string result = ClickHouse::punycodeEncode(input);
    std::cout << result << std::endl;
#endif




    // const std::string testAddr {"https://ввв.ya.рф/"};
    // const std::string testAddr {"https://привет.yandex.тестовое.задание.com22/"};
    // const std::string testAddr {"https://aaa.bbb.ccc/"};
    // std::cout << ClickHouse::punycodeEncodeFinalUTF8(testAddr) << std::endl;
    // std::cout << ClickHouse::punycodeEncodeFinalUTF8_EX(testAddr) << std::endl; // INFO: Use this one
    // std::cout << ClickHouse::punycodeEncodeFinalUTF8_EX_FAST(testAddr) << std::endl;



    // INFO: Extract DNS
#if 0
    const std::string text {"https://дом.рф/gvcggfc/"};
    std::string::size_type start = text.find("://");
    start = std::string::npos == start ? 0 : start + 3;

    std::string::size_type last = text.find('/', start);
    last = std::string::npos == last ? text.size() : last;

    std::cout << start << " - " << last << std::endl;
    std::cout << std::string(text, start, last - start) << std::endl;
    std::cout << text << std::endl;
#endif

#if 0
    {
        // const std::u32string u32Str { 0x63 ,0x6f ,0x6d, 0x424 };
        const std::string u8Str { "какой-то.длинный.домен.comZZя" };
        const std::u32string u32Str = ClickHouse::to_utf32(u8Str);

        // std::cout << ClickHouse::to_utf8(u32Str) << "  " << u8Str << std::endl;
        // ClickHouse::encodeUtf32Str(u32Str);

        const std::string result1 = ClickHouse::punycodeEncode(u32Str);
        std::cout << result1 << std::endl;

        const std::string result2 = ClickHouse::punycodeEncode1(u8Str);
        std::cout << result2 << std::endl;
    }
#endif

#if 0
    {
        // какой-то.длинный.домен.com
        std::u32string utf32str {0x43a ,0x430 ,0x43a ,0x43e ,0x439 ,0x02d ,0x442 ,0x43e ,
                              0x02e ,0x434 ,0x43b ,0x438 ,0x43d ,0x43d ,0x44b ,0x439 ,
                              0x02e ,0x434 ,0x43e ,0x43c ,0x435 ,0x43d ,0x02e ,0x063 ,0x06f ,0x06d};

        auto s = ClickHouse::to_utf8(utf32str);
        std::cout << s << std::endl;
        std::cout << std::boolalpha << isASCII(s) << "  " << isASCII2(s) << std::endl;
    }
#endif

#if 1
    const std::string testString {"https://ввв.яндекс.рф.привет.рф.привет.рф.привет.рф.привет/"};
    std::u32string u32TestString = ClickHouse::to_utf32(testString);

    {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 2'000'000; ++i) {
            // ClickHouse::punycodeEncode(u32TestString);
            // ClickHouse::punycodeEncodeFinal(u32TestString);
            // ClickHouse::punycodeEncodeFinalUTF8(testString);
            ClickHouse::punycodeEncodeFinalUTF8_EX(testString);
            // ClickHouse::punycodeEncode1(u32TestString);
        }
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "It took me " << time_span.count() << " seconds.\n";
    }

    {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 2'000'000; ++i) {
            ClickHouse::punycodeEncodeFinalUTF8_EX_FAST(testString);
        }
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "It took me " << time_span.count() << " seconds.\n";
    }
#endif
};
