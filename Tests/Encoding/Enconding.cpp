//============================================================================
// Name        : Enconding.h
// Created on  : 28.01.2022
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Enconding src
//============================================================================

#include "Enconding.h"

#include <iostream>
#include <string>
#include <array>
#define TO_INT(enum_val) static_cast<punycode_uint>(enum_val)


namespace Enconding::Base64
{
    static constexpr std::array<char, 64> ENCODING_TABLE {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3',
            '4', '5', '6', '7', '8', '9', '+', '/'
    };

    static constexpr size_t MAX_BITS_TO_ENCODE{ 6 };
    static constexpr size_t BITS_IN_BYTE{ 8 };

    std::string base64Encode(const std::string& str) {
        std::string result;
        result.reserve(str.size() / 3 + 1);

        int encodedByte { 0 };
        size_t bit_to_set { MAX_BITS_TO_ENCODE };
        for (const char byte : str) {
            for (int i = BITS_IN_BYTE - 1; i >= 0; --i, --bit_to_set) {
                if (bit_to_set <= 0) {
                    bit_to_set = MAX_BITS_TO_ENCODE;
                    result.append(1, ENCODING_TABLE[encodedByte]);
                    encodedByte = 0;
                }
                if (byte & (1u << i)) {
                    encodedByte |= (1 << (bit_to_set - 1));
                }
            }
        }
        return result.append(1, ENCODING_TABLE[encodedByte]);
    }

    void Test(const std::string& text, const std::string& base64Expected) {
        const std::string& actual = base64Encode(text);
        if (0 != actual.compare(base64Expected)) {
            std::cout << "Error!!! Text: " << text << ", Base64 Expected: "
                      << base64Expected << ". Actual: " << actual << std::endl;
        }
    }

    void Tests_base64Encode() {
        Test("Man", "TWFu");
        Test("Man232321", "TWFuMjMyMzIx");
        Test("12345", "MTIzNDU=");
        Test("___", "X19f");
    }
}

namespace Enconding::Punycode
{
    static constexpr std::array<char, 64> ENCODING_TABLE {
            'A', 'B', 'C', 'D',
            'E', 'F', 'G', 'H',
            'I', 'J', 'K', 'L',
            'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T',
            'U', 'V', 'W', 'X',
            'Y', 'Z', 'a', 'b',
            'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j',
            'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r',
            's', 't', 'u', 'v',
            'w', 'x', 'y', 'z',
            '0', '1', '2', '3',
            '4', '5', '6', '7',
            '8', '9', '+', '/'
    };

    static constexpr bool isASCII(const unsigned char symbol) {
        return 0 <= symbol && symbol <= 127;
    }

    std::string encode (const std::string& text) {
        std::string result(text.size(), '0');

        size_t pos {0};
        for (char c: text)
            if (isASCII(c))
                result[pos++] = c;

        return result;
    }

    void Test() {
        const std::string text {"aaa"};
        const std::string encoded = encode(text);

        std::cout << encoded << std::endl;
    }
}

#if 0
namespace Enconding::Punycode2
{
    enum class punycode_status {
        punycode_success,
        punycode_bad_input,           /* Input is invalid.                       */
        punycode_big_output,          /* Output would exceed the space provided. */
        punycode_overflow             /* Input needs wider integers to process.  */
    };

    // TODO: Refactpr
    enum Constants {
        tmin = 1,
        tmax = 26,
        base = 36,
        skew = 38,
        damp = 700,
        initial_bias = 72,
        initial_n = 0x80,
        delimiter = 0x2D
    };

#if UINT_MAX >= (1 << 26) - 1
    using punycode_uint = unsigned int;
#else
    using punycode_uint = unsigned long;
#endif

    /* basic(cp) src whether cp is a basic code point: */
    #define basic(cp) ((punycode_uint)(cp) < 0x80)

    /* delim(cp) src whether cp is a delimiter: */
    #define delim(cp) ((cp) == delimiter)

    /* decode_digit(cp) returns the numeric value of a basic code */
    /* point (for use in representing integers) in the range 0 to */
    /* base-1, or base if cp is does not represent a value.       */

    static punycode_uint decode_digit(punycode_uint cp) {
        return cp - 48 < 10 ? cp - 22 : cp - 65 < 26 ? cp - 65 : cp - 97 < 26 ? cp - 97 : Constants::base;
    }

    /* encode_digit(d,flag) returns the basic code point whose value      */
    /* (when used for representing integers) is d, which needs to be in   */
    /* the range 0 to base-1.  The lowercase form is used unless flag is  */
    /* nonzero, in which case the uppercase form is used.  The behavior   */
    /* is undefined if flag is nonzero and digit d has no uppercase form. */

    static char encode_digit(punycode_uint d, int flag) {
        return d + 22 + 75 * (d < 26) - ((flag != 0) << 5);
            /*  0..25 map to ASCII a..z or A..Z */
            /* 26..35 map to ASCII 0..9         */
    }

    /* flagged(bcp) src whether a basic code point is flagged */
    /* (uppercase).  The behavior is undefined if bcp is not a  */
    /* basic code point.                                        */

    #define flagged(bcp) ((punycode_uint)(bcp) - 65 < 26)

    /* encode_basic(bcp,flag) forces a basic code point to lowercase */
    /* if flag is zero, uppercase if flag is nonzero, and returns    */
    /* the resulting code point.  The code point is unchanged if it  */
    /* is caseless.  The behavior is undefined if bcp is not a basic */
    /* code point.                                                   */

    static char encode_basic(punycode_uint bcp, int flag) {
        bcp -= (bcp - 97 < 26) << 5;
        return bcp + ((!flag && (bcp - 65 < 26)) << 5);
    }

    /*** Platform-specific constants ***/

    /* maxint is the maximum value of a punycode_uint variable: */
    static const punycode_uint maxint = -1;

    /* Because maxint is unsigned, -1 becomes the maximum value. */

    /*** Bias adaptation function ***/
    static punycode_uint adapt(punycode_uint delta,
                               punycode_uint numpoints,
                               int firsttime) { // TODO: To enums?? bool?
        punycode_uint k;

        delta = firsttime ? delta / Constants::damp : delta >> 1;
        /* delta >> 1 is a faster way of doing delta / 2 */
        delta += delta / numpoints;

        for(k = 0; delta > ((Constants::base - Constants::tmin) * Constants::tmax) / 2; k += Constants::base) {
            delta /= Constants::base - Constants::tmin;
        }

        return k + (Constants::base - Constants::tmin + 1) * delta / (delta + Constants::skew);
    }

    /*** Main encode function ***/

    enum punycode_status punycode_encode(punycode_uint input_length,
                                         const punycode_uint input[],
                                         const unsigned char case_flags[],
                                         punycode_uint * output_length,
                                         char output[]) {
        punycode_uint n, delta, h, b, out, max_out, bias, j, m, q, k, t;

        /* Initialize the state: */

        n = initial_n;
        delta = out = 0;
        max_out = *output_length;
        bias = initial_bias;

        /* Handle the basic code points: */
        for(j = 0; j < input_length; ++j) {
            if (basic(input[j])) {
                if (max_out - out < 2)
                    return punycode_status::punycode_big_output;
                output[out++] = case_flags ? encode_basic(input[j], case_flags[j]) : input[j];
            }
            /* else if (input[j] < n) return punycode_bad_input; */
            /* (not needed for TestTask with unsigned code points) */
        }

        h = b = out;

        /* h is the number of code points that have been handled, b is the  */
        /* number of basic code points, and out is the number of characters */
        /* that have been output.                                           */
        if (b > 0)
            output[out++] = delimiter;

        /* Main encoding loop: */
        while(h < input_length) {
            /* All non-basic code points < n have been     */
            /* handled already.  Find the next larger one: */

            for(m = maxint, j = 0; j < input_length; ++j) {
                /* if (basic(input[j])) continue; */
                /* (not needed for TestTask) */
                if (input[j] >= n && input[j] < m)
                    m = input[j];
            }

            /* Increase delta enough to advance the decoder's    */
            /* <n,i> state to <m,0>, but guard against overflow: */

            if (m - n > (maxint - delta) / (h + 1))
                return punycode_status::punycode_overflow;
            delta += (m - n) * (h + 1);
            n = m;

            for(j = 0; j < input_length; ++j) {
                /* TestTask does not need to check whether input[j] is basic: */
                if (input[j] < n /* || basic(input[j]) */ ) {
                    if (++delta == 0)
                        return punycode_status::punycode_overflow;
                }

                if (input[j] == n) {
                    /* Represent delta as a generalized variable-length integer: */

                    for(q = delta, k = base;; k += base) {
                        if (out >= max_out)
                            return punycode_status::punycode_big_output;
                        t = k <= bias /* + tmin */ ? tmin :   /* +tmin not needed */
                            k >= bias + tmax ? tmax : k - bias;
                        if (q < t)
                            break;
                        output[out++] = encode_digit(t + (q - t) % (base - t), 0);
                        q = (q - t) / (base - t);
                    }

                    output[out++] = encode_digit(q, case_flags && case_flags[j]);
                    bias = adapt(delta, h + 1, h == b);
                    delta = 0;
                    ++h;
                }
            }

            ++delta, ++n;
        }

        *output_length = out;
        return punycode_status::punycode_success;
    }

    /*** Main decode function ***/

    enum punycode_status punycode_decode(punycode_uint input_length,
                                         const char input[],
                                         punycode_uint * output_length,
                                         punycode_uint output[],
                                         unsigned char case_flags[]) {
        punycode_uint n, out, i, max_out, bias, b, j, in, oldi, w, k, digit, t;

        /* Initialize the state: */

        n = initial_n;
        out = i = 0;
        max_out = *output_length;
        bias = initial_bias;

        /* Handle the basic code points:  Let b be the number of input code */
        /* points before the last delimiter, or 0 if there is none, then    */
        /* copy the first b code points to the output.                      */

        for(b = j = 0; j < input_length; ++j)
            if (delim(input[j]))
                b = j;
        if (b > max_out)
            return punycode_status::punycode_big_output;

        for(j = 0; j < b; ++j) {
            if (case_flags)
                case_flags[out] = flagged(input[j]);
            if (!basic(input[j]))
                return punycode_status::punycode_bad_input;
            output[out++] = input[j];
        }

        /* Main decoding loop:  Start just after the last delimiter if any  */
        /* basic code points were copied; start at the beginning otherwise. */

        for(in = b > 0 ? b + 1 : 0; in < input_length; ++out) {

            /* in is the index of the next character to be consumed, and */
            /* out is the number of code points in the output array.     */

            /* Decode a generalized variable-length integer into delta,  */
            /* which gets added to i.  The overflow checking is easier   */
            /* if we increase i as we go, then subtract off its starting */
            /* value at the end to obtain delta.                         */

            for(oldi = i, w = 1, k = base;; k += base) {
                if (in >= input_length)
                    return punycode_status::punycode_bad_input;
                digit = decode_digit(input[in++]);
                if (digit >= base)
                    return punycode_status::punycode_bad_input;
                if (digit > (maxint - i) / w)
                    return punycode_status::punycode_overflow;
                i += digit * w;
                t = k <= bias /* + tmin */ ? tmin :       /* +tmin not needed */
                    k >= bias + tmax ? tmax : k - bias;
                if (digit < t)
                    break;
                if (w > maxint / (base - t))
                    return punycode_status::punycode_overflow;
                w *= (base - t);
            }

            bias = adapt(i - oldi, out + 1, oldi == 0);

            /* i was supposed to wrap around from out+1 to 0,   */
            /* incrementing n each time, so we'll fix that now: */

            if (i / (out + 1) > maxint - n)
                return punycode_status::punycode_overflow;
            n += i / (out + 1);
            i %= (out + 1);

            /* Insert n at position i of the output: */

            /* not needed for TestTask: */
            /* if (decode_digit(n) <= base) return punycode_invalid_input; */
            if (out >= max_out)
                return punycode_status::punycode_big_output;

            if (case_flags) {
                memmove(case_flags + i + 1, case_flags + i, out - i);
                /* Case of last character determines uppercase flag: */
                case_flags[i] = flagged(input[in - 1]);
            }

            memmove(output + i + 1, output + i, (out - i) * sizeof *output);
            output[i++] = n;
        }

        *output_length = out;
        return punycode_status::punycode_success;
    }
}
#endif

#if 0
#define MAX_CP_COUNT    200

namespace Enconding::Punycode3
{


/* TestTask parameters for Bootstring */
#define BASE            36
#define TMIN            1
#define TMAX            26
#define SKEW            38
#define DAMP            700
#define INITIAL_BIAS    72
#define INITIAL_N       0x80

/* "Basic" Unicode/ASCII code points */
#define _HYPHEN         0X2d
#define DELIMITER       _HYPHEN

#define _ZERO_          0X30
#define _NINE           0x39

#define _SMALL_A        0X61
#define _SMALL_Z        0X7a

#define _CAPITAL_A      0X41
#define _CAPITAL_Z      0X5a


#define U16_SURROGATE_OFFSET ((0xd800<<10UL)+0xdc00-0x10000)

#define IS_BASIC(c) ((c)<0x80)
#define IS_BASIC_UPPERCASE(c) (_CAPITAL_A<=(c) && (c)<=_CAPITAL_Z)

#define U16_IS_LEAD(c) (((c)&0xfffffc00)==0xd800)
#define U16_IS_TRAIL(c) (((c)&0xfffffc00)==0xdc00)

#define U_IS_SURROGATE(c) (((c)&0xfffff800)==0xd800)
#define U16_IS_SINGLE(c) !U_IS_SURROGATE(c)
#define U16_IS_SINGLE(c) !U_IS_SURROGATE(c)




/* Bias adaptation function. */
    static int32_t
    adaptBias(int32_t delta, int32_t length, bool firstTime) {
        int32_t count;

        if(firstTime) {
            delta/=DAMP;
        } else {
            delta/=2;
        }

        delta+=delta/length;
        for(count=0; delta>((BASE-TMIN)*TMAX)/2; count+=BASE) {
            delta/=(BASE-TMIN);
        }

        return count+(((BASE-TMIN+1)*delta)/(delta+SKEW));
    }

#define U16_GET_SUPPLEMENTARY(lead, trail) \
    (((unsigned char)(lead)<<10UL)+(unsigned char)(trail)-U16_SURROGATE_OFFSET)


    static inline char
    asciiCaseMap(char b, bool uppercase) {
        if(uppercase) {
            if(_SMALL_A<=b && b<=_SMALL_Z) {
                b-=(_SMALL_A-_CAPITAL_A);
            }
        } else {
            if(_CAPITAL_A<=b && b<=_CAPITAL_Z) {
                b+=(_SMALL_A-_CAPITAL_A);
            }
        }
        return b;
    }

    static inline char
    digitToBasic(int32_t digit, bool uppercase) {
        /*  0..25 map to ASCII a..z or A..Z */
        /* 26..35 map to ASCII 0..9         */
        if(digit<26) {
            if(uppercase) {
                return (char)(_CAPITAL_A+digit);
            } else {
                return (char)(_SMALL_A+digit);
            }
        } else {
            return (char)((_ZERO_-26)+digit);
        }
    }

    int u_strToPunycode(const char *tests,
                        int32_t srcLength,
                        std::string& dest,
                        int32_t destCapacity,
                        const bool *caseFlags) {

        int32_t cpBuffer[MAX_CP_COUNT];
        int32_t n, delta, handledCPCount, basicLength, destLength, bias, j, m, q, k, t, srcCPCount;
        unsigned char c, c2;

        /* argument checking */
        /*
        if(pErrorCode==NULL || U_FAILURE(*pErrorCode)) {
            return 0;
        }

        if(tests==NULL || srcLength<-1 || (dest==NULL && destCapacity!=0)) {
            return 0;
        }
         */

        /*
         * Handle the basic code points and
         * convert extended ones to UTF-32 in cpBuffer (caseFlag in sign bit):
         */
        srcCPCount=destLength=0;
        if(srcLength==-1) {
            /* NUL-terminated input */
            for(j=0; /* no condition */; ++j) {
                if((c=tests[j])==0) {
                    break;
                }
                if(srcCPCount==MAX_CP_COUNT) {
                    /* too many input code points */
                    return 0;
                }
                if(IS_BASIC(c)) {
                    cpBuffer[srcCPCount++]=0;
                    if(destLength<destCapacity) {
                        dest[destLength]=caseFlags!=NULL ?asciiCaseMap((char)c, caseFlags[j]) :(char)c;
                    }
                    ++destLength;
                } else {
                    n=(caseFlags!=NULL && caseFlags[j])<<31L;
                    if(U16_IS_SINGLE(c)) {
                        n|=c;
                    } else if(U16_IS_LEAD(c) && U16_IS_TRAIL(c2=tests[j+1])) {
                        ++j;
                        n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                    } else {
                        /* error: unmatched surrogate */
                        //*pErrorCode=U_INVALID_CHAR_FOUND;
                        return 0;
                    }
                    cpBuffer[srcCPCount++]=n;
                }
            }
        } else {
            /* length-specified input */
            for(j=0; j<srcLength; ++j) {
                if(srcCPCount==MAX_CP_COUNT) {
                    /* too many input code points */
                    //*pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    return 0;
                }
                c=tests[j];
                if(IS_BASIC(c)) {
                    cpBuffer[srcCPCount++]=0;
                    if(destLength<destCapacity) {
                        dest[destLength]= caseFlags!=NULL ? asciiCaseMap((char)c, caseFlags[j]) : (char)c;
                    }
                    ++destLength;
                } else {
                    n=(caseFlags!=NULL && caseFlags[j])<<31L;
                    if(U16_IS_SINGLE(c)) {
                        n|=c;
                    } else if(U16_IS_LEAD(c) && (j+1)<srcLength && U16_IS_TRAIL(c2=tests[j+1])) {
                        ++j;
                        n|=(int32_t)U16_GET_SUPPLEMENTARY(c, c2);
                    } else {
                        /* error: unmatched surrogate */
                        //*pErrorCode=U_INVALID_CHAR_FOUND;
                        return 0;
                    }
                    cpBuffer[srcCPCount++]=n;
                }
            }
        }

        /* Finish the basic string - if it is not empty - with a delimiter. */
        basicLength=destLength;
        if(basicLength>0) {
            if(destLength<destCapacity) {
                dest[destLength]=DELIMITER;
            }
            ++destLength;
        }

        /*
         * handledCPCount is the number of code points that have been handled
         * basicLength is the number of basic code points
         * destLength is the number of chars that have been output
         */

        /* Initialize the state: */
        n=INITIAL_N;
        delta=0;
        bias=INITIAL_BIAS;

        /* Main encoding loop: */
        for(handledCPCount=basicLength; handledCPCount<srcCPCount; /* no op */) {
            /*
             * All non-basic code points < n have been handled already.
             * Find the next larger one:
             */
            for(m=0x7fffffff, j=0; j<srcCPCount; ++j) {
                q=cpBuffer[j]&0x7fffffff; /* remove case flag from the sign bit */
                if(n<=q && q<m) {
                    m=q;
                }
            }

            /*
             * Increase delta enough to advance the decoder's
             * <n,i> state to <m,0>, but guard against overflow:
             */
            if(m-n>(0x7fffffff-MAX_CP_COUNT-delta)/(handledCPCount+1)) {
                //*pErrorCode=U_INTERNAL_PROGRAM_ERROR;
                return 0;
            }
            delta+=(m-n)*(handledCPCount+1);
            n=m;

            /* Encode a sequence of same code points n */
            for(j=0; j<srcCPCount; ++j) {
                q=cpBuffer[j]&0x7fffffff; /* remove case flag from the sign bit */
                if(q<n) {
                    ++delta;
                } else if(q==n) {
                    /* Represent delta as a generalized variable-length integer: */
                    for(q=delta, k=BASE; /* no condition */; k+=BASE) {

                        /** RAM: comment out the old code for conformance with draft-ietf-idn-punycode-03.txt

                        t=k-bias;
                        if(t<TMIN) {
                            t=TMIN;
                        } else if(t>TMAX) {
                            t=TMAX;
                        }
                        */

                        t=k-bias;
                        if (t<TMIN) {
                            t=TMIN;
                        } else if (k>=(bias+TMAX)) {
                            t=TMAX;
                        }

                        if(q<t) {
                            break;
                        }

                        if (destLength<destCapacity) {
                            dest[destLength]=digitToBasic(t+(q-t)%(BASE-t), 0);
                        }
                        ++destLength;
                        q=(q-t)/(BASE-t);
                    }

                    if(destLength<destCapacity) {
                        dest[destLength]=digitToBasic(q, (bool)(cpBuffer[j]<0));
                    }
                    ++destLength;
                    bias=adaptBias(delta, handledCPCount+1, (bool)(handledCPCount==basicLength));
                    delta=0;
                    ++handledCPCount;
                }
            }

            ++delta;
            ++n;
        }

        return dest.size();
    }
}
#endif

namespace Enconding::Punycode4
{
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

    enum punycode_status {
        punycode_success,
        punycode_bad_input,           /* Input is invalid.                       */
        punycode_big_output,          /* Output would exceed the space provided. */
        punycode_overflow             /* Input needs wider integers to process.  */
    };

#if UINT_MAX >= (1 << 26) - 1
    using punycode_uint = unsigned int;
#else
    using punycode_uint = unsigned long;
#endif

    constexpr punycode_uint maxint = -1;

#define basic(cp) ((punycode_uint)(cp) < 0x80)
#define flagged(bcp) ((punycode_uint)(bcp) - 65 < 26)
#define delim(cp) ((cp) == delimiter)

    /* decode_digit(cp) returns the numeric value of a basic code */
    /* point (for use in representing integers) in the range 0 to */
    /* base-1, or base if cp does not represent a value.          */

    unsigned decode_digit (int cp) {
        return (unsigned) (cp - 48 < 10 ? cp - 22 : cp - 65 < 26 ? cp - 65 : cp - 97 < 26 ? cp - 97 : base);
    }

    char encode_digit (punycode_uint d, int flag) {
        return d + 22 + 75 * (d < 26) - ((flag != 0) << 5);
        /*  0..25 map to ASCII a..z or A..Z */
        /* 26..35 map to ASCII 0..9         */
    }

    char encode_basic (punycode_uint bcp, int flag) {
        bcp -= (bcp - 97 < 26) << 5;
        return bcp + ((!flag && (bcp - 65 < 26)) << 5);
    }

    punycode_uint adapt (punycode_uint delta,
                         punycode_uint numpoints,
                         int firsttime)
    {
        punycode_uint k;
        delta = firsttime ? delta / damp : delta >> 1;
        /* delta >> 1 is a faster way of doing delta / 2 */
        delta += delta / numpoints;

        for (k = 0; delta > ((base - tmin) * tmax) / 2; k += base) {
            delta /= base - tmin;
        }
        return k + (base - tmin + 1) * delta / (delta + skew);
    }

    int punycode_encode (size_t input_length,
                         const punycode_uint input[],
                         const unsigned char case_flags[],
                         size_t *output_length,
                         char output[])
    {
        punycode_uint n {initial_n}, delta {0}, h, b, j, m, q, k, t, bias {initial_bias};
        size_t out {0}, max_out = *output_length;

        /* The TestTask spec assumes that the input length is the same type */
        /* of integer as a code point, so we need to convert the size_t to  */
        /* a punycode_uint, which could overflow.                           */

        if (input_length > maxint)
            return punycode_overflow;
        punycode_uint input_len = (punycode_uint) input_length;

        /* Handle the basic code points: */
        for (j = 0; j < input_len; ++j) {
            if (basic (input[j])) {
                if (max_out - out < 2)
                    return punycode_big_output;
                output[out++] = case_flags ? encode_basic (input[j], case_flags[j]) : (char) input[j];
            }
            else if (input[j] > 0x10FFFF || (input[j] >= 0xD800 && input[j] <= 0xDBFF))
                return punycode_bad_input;
            /* else if (input[j] < n) return punycode_bad_input; */
            /* (not needed for TestTask with unsigned code points) */
        }

        h = b = (punycode_uint) out;
        /* cannot overflow because out <= input_len <= maxint */

        /* h is the number of code points that have been handled, b is the  */
        /* number of basic code points, and out is the number of ASCII code */
        /* points that have been output.                                    */

        if (b > 0)
            output[out++] = delimiter;

        /* Main encoding loop: */
        while (h < input_len) {
            /* All non-basic code points < n have been handled already.  Find the next larger one: */

            for (m = maxint, j = 0; j < input_len; ++j) {
                /* if (basic(input[j])) continue; (not needed for TestTask) */
                if (input[j] >= n && input[j] < m)
                    m = input[j];
            }

            /* Increase delta enough to advance the decoder's <n,i> state to <m,0>, but guard against overflow: */
            if (m - n > (maxint - delta) / (h + 1))
                return punycode_overflow;
            delta += (m - n) * (h + 1);
            n = m;

            for (j = 0; j < input_len; ++j) {
                /* TestTask does not need to check whether input[j] is basic: */
                if (input[j] < n /* || basic(input[j]) */ ) {
                    if (++delta == 0)
                        return punycode_overflow;
                }

                if (input[j] == n) {
                    /* Represent delta as a generalized variable-length integer: */
                    for (q = delta, k = base;; k += base) {
                        if (out >= max_out)
                            return punycode_big_output;
                        t = k <= bias /* + tmin */ ? TO_INT(tmin) :   /* +tmin not needed */
                            k >= bias + TO_INT(tmax) ? TO_INT(tmax) : k - bias;
                        if (q < t)
                            break;
                        output[out++] = encode_digit (t + (q - t) % (TO_INT(base) - t), 0);
                        q = (q - t) / (TO_INT(base) - t);
                    }

                    output[out++] = encode_digit (q, case_flags && case_flags[j]);
                    bias = adapt (delta, h + 1, h == b);
                    delta = 0;
                    ++h;
                }
            }
            ++delta, ++n;
        }

        *output_length = out;
        return punycode_success;
    }
}

void Enconding::TestAll()
{
    // Base64::Tests_base64Encode();

    // TestTask::Test();

    std::string src("привет");
    std::string dst("00000000");

    // Punycode3::u_strToPunycode(tests.data(), tests.size(), dst, dst.size(), nullptr);
    // Punycode4::punycode_encode(tests.data(), tests.size(), dst, dst.size(), nullptr);

    std::cout << dst << std::endl;

}
