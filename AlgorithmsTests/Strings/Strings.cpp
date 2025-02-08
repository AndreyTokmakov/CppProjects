/**============================================================================
Name        : Strings.cpp
Created on  : 08.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Strings.cpp
============================================================================**/

#include "Strings.h"
#include "../Utilities/Utilities.h"
#include "../Utilities/StringUtilities.h"

#include <iostream>
#include <string>
#include <string_view>

#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>


namespace
{
    using StringPair = std::pair<std::string, std::string>;
    using IntPair = std::pair<int, int>;

    template<typename T>
    std::ostream &operator<<(std::ostream &stream, const std::vector<T> &list)
    {
        for (const auto &i: list)
            stream << " " << i;
        return stream;
    }
}


namespace Strings
{

    size_t find_common_prefix_and_postfix(const std::string& str)
    {
        size_t maxLen = 0;
        /** idx = (str.size() + 1) / 2 ---> starting from the center + 1 **/
        /** (str.size() + 1) / 2    <==>  str.size() / 2 + str.size() % 2  **/
        for (size_t idx = (str.size() + 1) / 2; idx < str.size(); ++idx)
        {
            if (size_t left = 0, right = idx, len = 1; str[left] == str[right])
            {
                while (str[++left] == str[++right]) {
                    ++len;
                }
                maxLen = std::max(maxLen, len);
            }
        }
        return maxLen;
    }

    // https://www.geeksforgeeks.org/longest-prefix-also-suffix/
    // Efficient Solution: The idea is to use the preprocessing algorithm KMP search.
    // In the preprocessing algorithm, we build lps array which stores the following values.
    size_t find_common_prefix_and_postfix_efficient(const std::string& str)
    {
        const size_t size = str.size();
        std::vector<size_t> lps(size);

        // length of the previous longest prefix suffix , the loop calculates lps[i] for i = 1 to n-1
        for (size_t idx = (size + 1) / 2, length = 0; idx < size; /** **/)
        {
            if (str[idx] == str[length])
            {
                length++;
                lps[idx] = length;
                idx++;
            }
            else // (pat[i] != pat[len])
            {
                // This is tricky. Consider the example. AAACAAAA and i = 7. The idea is similar to search step.
                if (length != 0)
                {
                    length = lps[length - 1]; // Also, note that we do not increment i here
                }
                else // if (len == 0)
                {
                    lps[idx] = 0;
                    idx++;
                }
            }
        }

        // Since we are looking for non overlapping parts.
        return lps[size - 1];
    }

    int64_t find_common_prefix_and_postfix_efficient2(const std::string& str)
    {
        const int64_t size = std::ssize(str);
        if (str.empty())
            return 0;

        int64_t endSuffix = size - 1, endPrefix = size/2 - 1;
        while (endPrefix >= 0)
        {
            if (str[endPrefix] != str[endSuffix])
            {
                if (endSuffix != size - 1) {
                    endSuffix = size - 1;
                }
                else{
                    --endPrefix;
                }
            }
            else{
                --endSuffix;
                --endPrefix;
            }
        }

        return size - endSuffix - 1;
    }


    /**
    Given a string s, find the length of the longest prefix, which is also a suffix. The prefix and suffix should not overlap.
    aabcdaabc --> 4 [The string "aabc" is the longest]
    abcab     --> 2 [The string "ab" is the longest]
    aaaa      --> 2 [The string "aa" is the longest]
    **/

    void FindCommon_PrefixAndPostfix()
    {

        for (const std::string& str: {"123X412", "aaaa", "aabaa", "aabcaa", "aacbaac", "aabcdaabc"})
        {
            std::cout << find_common_prefix_and_postfix(str) << "  "
                      << find_common_prefix_and_postfix_efficient(str) << "  "
                      << find_common_prefix_and_postfix_efficient2(str)
                      << std::endl;
        }
    }
}

namespace Strings
{
    std::vector<std::string> _top_K_Frequent(const std::vector<std::string>& words,
                                             int k)
    {
        std::unordered_map<std::string, uint32_t> wordsCount;
        for (const std::string& s: words)
            ++wordsCount[s];

        std::map<uint32_t, std::vector<std::string>> result;
        for (const auto& [str, v]: wordsCount)
            result[v].push_back(str);

        std::vector<std::string> out;
        out.reserve(k);
        for (auto iter = result.rbegin(); result.rend() != iter; ++iter) {
            std::sort(iter->second.begin(), iter->second.end());
            for (auto &&s: iter->second) {
                out.push_back(std::move(s));
                if (--k == 0)
                    return out;
            }
        }
        return out;
    }

    void topKFrequent()
    {
        const std::vector<std::string> words {"i", "love", "leetcode", "i", "love", "coding"};
        std::vector<std::string> result = _top_K_Frequent(words, 3);
        std::cout << result << std::endl;
    }
}


namespace Strings
{
    using Utilities::ScopedTimer;

    int find_last_not_of(const std::string& str, const std::string& txt)
    {
        bool chars[256] {};
        for (char c: txt)
            chars[static_cast<uint8_t>(c)] = true;

        for (int i = str.size() - 1; i >= 0; --i) {
            if (chars[str[i]])
                return i;
        }
        return -1;
    }

    int find_last_not_of_less_mem(const std::string& str, const std::string& txt)
    {
        uint8_t chars[32] {};
        for (const uint8_t charNum: txt)
            chars[charNum / 8] |= (1 << charNum % 8);

        for (int i = static_cast<int>(str.size() - 1); i >= 0; --i) {
            if (chars[str[i] / 8] & (1u << (str[i] % 8)))
                return i;
        }
        return -1;
    }

    void FindLastNotOf__Benchmark()
    {
        constexpr size_t testsCount = 25'000;

        {
            ScopedTimer timer ("Test1");
            for (size_t n = 0; n < testsCount; ++n)
                for (size_t i = 0; i < testsCount; ++i)
                    find_last_not_of("dsdadsadasd454656ijsid837r374343743", "abcxcxc");
        }
        {
            ScopedTimer timer ("Test2");
            for (size_t n = 0; n < testsCount; ++n)
                for (size_t i = 0; i < testsCount; ++i)
                    find_last_not_of_less_mem("dsdadsadasd454656ijsid837r374343743", "abcxcxc");
        }
    }
}


namespace Strings
{
    void printAllSubstrings(const std::string& str)
    {
        for (size_t size = str.size(), i = 0; i < size; ++i) {
            for (size_t n = i + 1; n <= size; ++n)
                std::cout << std::string_view(str.data() + i, n - i) << std::endl;
        }
    }

    void PrintAllSubStrings()
    {
        printAllSubstrings("101");
    }
}

namespace Strings
{
    int count_binary_substrings(const std::string& str)
    {
        int result = 0;
        for (int size = str.size(), i = 0; i < size; ++i) {
            for (int n = i + 1, counter = 0; n <= size; ++n, counter = 0)
            {
                if (2 > n - i)
                    continue;
                for (int idx = i; idx <= n - 1; ++idx)
                    counter += str[idx] == '1' ? 1 : -1;
                if (0 == counter)
                    ++result;
            }
        }
        return result;
    }

    void CountBinarySubstrings()
    {
        for (const std::string& str: std::vector<std::string>{
                "101", "1", "0", "1010", "10101"
        })
        {
            std::cout << str << " -> " << count_binary_substrings(str) << std::endl;
        }
    }
}

namespace Strings
{
    void split_to(const std::string &str,
                  std::vector<std::string_view>& params,
                  const std::string& delimiter = std::string {" "})
    {
        params.clear();
        size_t pos = 0, prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos) {
            params.emplace_back(str.data() + prev, pos - prev);
            prev = pos + delimiter.length();
        }
        params.emplace_back(str.data() + prev, str.length() - prev);
    }

    void reverse_words_in_string(const std::string& str)
    {
        std::vector<std::string_view> params;
        split_to(str, params);

        std::string result;
        for (auto it = params.rbegin(); it != params.rend(); ++it) {
            std::cout << *it << std::endl;
            result.append(std::string(*it)).append(" ");
        }

        result.erase(result.size() - 1);
        std::cout << std::quoted(result) << std::endl;
    }

    void Reverse_Words_in_String()
    {
        const std::string str { "the sky is blue" };
        const std::string str1 { "a  good   example   " };
        const std::string str2 { " asdasd df f"};

        // reverse_words_in_string(str);
        // reverse_words_in_string(str1);
        reverse_words_in_string(str2);
    }
}

namespace Strings
{
    void reverse_words_in_string2(const std::string& str)
    {
        std::string result;
        result.reserve(str.size());

        size_t pos = 0, prev = 0;
        while ((pos = str.find(' ', prev)) != std::string::npos) {
            if (pos > prev)
                result.insert(0, str, prev, pos - prev).insert(0, " ");
            prev = pos + 1;
        }
        result.insert(0, str, prev, str.length() - prev);

        result.shrink_to_fit();
        std::cout << std::quoted(result) << std::endl;
    }

    void Reverse_Words_in_String2()
    {
        const std::string str { "the sky is blue" };
        const std::string str1 { "a  good   example   " };
        const std::string str2 { " asdasd df f"};
        const std::string str3 { "  hello world  "};

        // reverse_words_in_string(str);
        // reverse_words_in_string(str1);
        // reverse_words_in_string2(str2);
        reverse_words_in_string2(str3);
    }
}


namespace Strings
{
    int Compare_Version_Numbers()
    {
        std::string version1 = "1.014.33", version2 = "1.014.33";

        version1 += ".1";
        version2 += ".1";

        int16_t v1 = 0, v2 = 0, strt1 = 0, strt2 = 0;
        std::pair<size_t, size_t> pos1 {0, pos1.second = version1.find('.')}, pos2 {0, version2.find('.')};
        while (std::string::npos != pos1.second && std::string::npos != pos2.second )
        {
            strt1 = version1.find_first_not_of('0', pos1.first);
            strt2 = version2.find_first_not_of('0', pos2.first);

            std::from_chars(version1.data() + strt1, version1.data() + pos1.second, v1);
            std::from_chars(version2.data() + strt2, version2.data() + pos2.second, v2);


            if (v1 != v2)
                return v1 > v2 ? 1 : -1;

            pos1.second = version1.find('.', pos1.first = pos1.second + 1);
            pos2.second = version2.find('.', pos2.first = pos2.second + 1);
        }
        return 0;
    }
}

namespace Strings
{
    bool __contains(const std::string& text,
                    const std::string& str)
    {
        for (int idx = 0, m = 0, n= 0; idx <= std::ssize(text) - std::ssize(str); ++idx)
        {
            // std::cout << idx << std::endl;
            for (m = 0, n = idx; m < str.size(); ++m, ++n) {
                // std::cout << '\t' << n << " - " << m << std::endl;
                if (str[m] != text[n])
                    break;
            }
            if (m == str.size())
                return true;
        }
        return false;
    }

    int find(const std::string& haystack, const std::string& needle)
    {
        const int textSize = std::ssize(haystack), searchBlockSize = std::ssize(needle);
        for (int idx = 0, m = 0, n = 0; idx <= textSize - searchBlockSize; ++idx)
        {
            for (m = 0, n = idx; m < searchBlockSize; ++m, ++n) {
                if (needle[m] != haystack[n])
                    break;
            }
            if (m == searchBlockSize)
                return idx;
        }
        return -1;
    }

    void Contains()
    {
        std::string text = "bcaa", to_find = "aa";

        std::cout << "Contains: " << std::boolalpha << __contains(text, to_find)
                  << ". Pos = "   << find(text, to_find) << std::endl;
    }
}

namespace Strings
{
    std::string longest_word(const std::string& input)
    {
        std::vector<std::string_view> words;
        StringUtilities::split_to(input, words, " ");

        size_t longestIdx = 0;
        for (size_t idx = 0, maxLen = 0; idx < words.size(); ++idx)
        {
            if (words[idx].size() > maxLen) {
                longestIdx = idx;
                maxLen = words[idx].size();
            }
        }

        return std::string{words[longestIdx]};
    }

    std::string longest_word_fast(const std::string& input)
    {
        uint32_t startMax {0}, endMax {0}, prev {0}, idx {};
        for (; idx < input.size(); ++idx) {
            if (' ' == input[idx]) {
                if (idx - prev > endMax - startMax) {
                    startMax = prev;
                    endMax = idx;
                }
                prev = idx + 1;
            }
        }

        if (idx - prev > endMax - startMax) {
            startMax = prev;
            endMax = idx;
        }

        return {input,startMax ,endMax - startMax};
    }

    void Longest_Word()
    {
        for (const auto  &[value, expected]: std::vector<StringPair> {
                {"11 222 33", "222"},
                {"1", "1"},
                {"", ""},
                {"Hello world aaaaaa bbbbbb", "aaaaaa"},

        })
        {
            const std::string actual1 = longest_word(value);
            if (expected != actual1) {
                std::cerr << expected << " != " << actual1 << std::endl;
                return;
            }

            const std::string actual2= longest_word_fast(value);
            if (expected != actual2) {
                std::cerr << expected << " != " << actual2 << std::endl;
                return;
            }
        }
        std::cout << "OK: All tests passed\n";
    }
}

namespace Strings
{
    std::string intersperse_strings(const std::string& str1,
                                    const std::string& str2)
    {
        std::string result {};
        result.reserve(str1.size() + str2.size());

        for (size_t idx1 = 0, idx2 = 0, size1 = str1.size(), size2 = str2.size(); idx1 < size1 || idx2 < size2;)
        {
            if (idx1 < size1 && idx2 < size2)
            {
                result.append(1, str1[idx1++]);
                result.append(1, str2[idx2++]);
            }
            else if (idx1 < size1)
                result.append(1, str1[idx1++]);
            else if (idx2 < size2)
                result.append(1, str2[idx2++]);
        }

        return result;
    }

    void Intersperse_String()
    {
        for (const auto  &[values, expected]: std::vector<std::pair<StringPair, std::string>> {
                {{"12345", "abcde"}, "1a2b3c4d5e"},
                {{"12345", "a"}, "1a2345"},
                {{"", "12345"}, "12345"},
                {{"abcd", ""}, "abcd"},
                {{"123456789", "z9y08regqa57"}, "1z293y40586r7e8g9qa57"},
        })
        {
            const std::string actual = intersperse_strings(values.first, values.second);
            if (expected != actual) {
                std::cerr << expected << " != " << actual << std::endl;
                return;
            }
        }
        std::cout << "OK: All tests passed\n";
    }
}

namespace Strings
{
    int are_anagrams(const std::string_view& str1,
                     const std::string_view& str2)
    {
        if (str1 == str2 || str1.length() != str2.length())
            return 0;

        int chars[256] = { 0 };
        for (char c : str1)
            chars[c]++;
        for (char c : str2)
            if (1 > chars[c]--)
                return 0;
        return 1;
    }

    int count_anagrams(const std::string& input)
    {
        std::vector<std::string_view> words;
        StringUtilities::split_to(input, words, " ");

        int count = 0;
        for (int i = 0; i < words.size(); ++i ){
            for (int n = 0; n < words.size(); ++n)
                count += are_anagrams(words[i], words[n]);
        }

        return count / 2;
    }

    /**
     * Have the function CountingAnagrams(str) take the str parameter and determine how many anagrams exist in the string.
     * An anagram is a new word that is produced from rearranging the characters in a different word,
     * Program should determine how many anagrams exist in a given string and return the total number.
     * For example: if str is "aa aa odg dog gdo" then your program should return 2 because "dog" and "gdo" are
     * anagrams of "odg".
     * The word "aa" occurs twice in the string but it isn't an anagram because it is the same word just repeated.
     * The string will contain only spaces and lowercase letters, no punctuation, numbers, or uppercase letters.
    */
    void Count_Anagrams()
    {
        for (const auto  &[value, expected]: std::vector<std::pair<std::string, int>> {
                {"cars are very cool so are arcs and my os", 2},
                {"a c b c run urn", 1},
                {"aa aa ab ba dog god", 2},
                {"ab ba run run run", 1},
                // {"abcd abdc bcda adbc abbc", 3},

        })
        {
            const int actual = count_anagrams(value);

            std::cout << actual << std::endl;
            if (expected != actual) {
                std::cerr << expected << " != " << actual << std::endl;
                return;
            }
        }
        std::cout << "OK: All tests passed\n";
    }
}

namespace Strings
{
    void destinationCity(const std::vector<StringPair>& paths)
    {
        std::string destCity {paths.front().second};
        std::vector<bool> taken (paths.size(), false);
        bool destExists {true};

        while (destExists)
        {
            destExists = false;
            for (size_t idx = 1; idx < paths.size(); ++idx)
            {
                if (taken[idx])
                    continue;

                std::cout << "{" << paths[idx].first << ", " << paths[idx].second << "} | " << destCity;
                if (paths[idx].first == destCity)
                {
                    destCity =  paths[idx].second;
                    std::cout << " | OK -> " << destCity << std::endl<< std::endl;
                    destExists = true;
                    taken[idx] = true;
                    break;
                }
                std::cout << std::endl;
            }
        }

        std::cout << std::endl << destCity << std::endl;
    }

    void destinationCity2(const std::vector<StringPair>& paths)
    {
        std::unordered_set<std::string_view> destinations;
        destinations.reserve(paths.size());

        for (const auto& [from, to]: paths)
            destinations.emplace(from);
        for (const auto& [from, to]: paths)
        {
            if (!destinations.contains(to)) {
                std::cout << to << std::endl;
                break;
            }
        }
    }

    /**
    You are given the array paths, where paths[i] = [cityAi, cityBi] means there exists a direct path going
    from cityAi to cityBi. Return the destination city, that is, the city without any path outgoing to another city.
    It is guaranteed that the graph of paths forms a line without any loop -> there will be exactly one destination city.
    */
    void DestinationCity()
    {
        destinationCity2({ {"London","New York"}, {"New York","Lima"}, {"Lima","Sao Paulo"}});
        destinationCity2({ {"B","C"}, {"D","B"}, {"C","A"}});
    }
}



namespace Strings
{
    bool is_interleaving_string_DEBUG(const std::string& str1,
                                      const std::string& str2,
                                      const std::string& dest)
    {
        int stopCounter = 0;

        int idx1 = -1, i1 = -1, n1 = -1, branch = 0;
        for (int idx = 0, i = 0, n = 0; idx < dest.size(); ++idx)
        {
            if (++stopCounter > 100) break;

            if (0 == branch && str1.size() > i && str1[i] == dest[idx])
            {
                if (-1 == idx1 && str2.size() > n && str1[i] == str2[n])
                {
                    std::cout << "MEMO (idx: " << idx << ", i: " << i << ", n: " << n << ") "
                              << "[" << dest[idx] << ", " << str1[i] << ", " << str2[n] << "]\n";
                    idx1 = idx; i1 = i; n1 = n;
                }

                std::cout << "dest[" << idx << "]:" <<  dest[idx] << " | using str1[" << i << "]:" << str1[i]
                          << " | (str2[" << n << "]: " << str2[n] << ") | branch: " << branch<< std::endl;
                ++i;
            }
            else if (str2.size() > n && str2[n] == dest[idx])
            {
                if (1 == branch) {
                    idx = idx1, i = i1, n = n1;
                    idx1 = -1; i1 = -1; n1 = -1;

                    std::cout << "USE RESTORE: (idx: " << idx << ", i: " << i << ", n: " << n << ")\n";
                    branch = 0;
                }
                std::cout << "dest[" << idx << "]:" <<  dest[idx] << " | using str2[" << n << "]:" << str2[n]
                          << " | (str1[" << i << "]: " << str1[i] << ") | branch: " << branch<< std::endl;
                ++n;

            }
            else
            {
                std::cout << "ELSE : dest[" << idx << "]: " <<  dest[idx] << " | str1[" << i << "]: "
                          << str1[i] << ", str2[" << n << "] = " << str2[n] << " | branch: " << branch << std::endl;

                if (-1 != idx1 && 0 == branch) {
                    branch = 1;
                    idx = idx1 - 1, i = i1, n = n1;

                    std::cout << "APPLY RESTORE (idx: " << idx1 << ", i: " << i1 << ", n: " << n1 << ")\n";
                }
                else
                    return false;

            }

        }
        return true;
    }


    bool is_interleaving_string(const std::string& str1,
                                const std::string& str2,
                                const std::string& dest)
    {
        if (dest.size() != str1.size() + str2.size())
            return false;

        int idx1 = -1, i1 = -1, n1 = -1, branch = 0;
        for (int idx = 0, i = 0, n = 0; idx < dest.size(); ++idx)
        {

            if (0 == branch && str1.size() > i && str1[i] == dest[idx])
            {
                if (-1 == idx1 && str2.size() > n && str1[i] == str2[n]) {
                    idx1 = idx; i1 = i; n1 = n;
                }
                ++i;
            }
            else if (str2.size() > n && str2[n] == dest[idx])
            {
                if (1 == branch) {
                    idx = idx1, i = i1, n = n1;
                    idx1 = -1; i1 = -1; n1 = -1;
                    branch = 0;
                }
                ++n;
            } else {
                if (-1 != idx1 && 0 == branch) {
                    branch = 1;
                }
                else
                    return false;
            }
        }
        return true;
    }

    void Interleaving_String()
    {
        std::cout << std::boolalpha << is_interleaving_string_DEBUG("aabcc", "dbbca", "aadbcbbcac") << std::endl;
        // std::cout << std::boolalpha << is_interleaving_string("aabcc", "dbbca", "aadbbbaccc") << std::endl;
        // std::cout << std::boolalpha << is_interleaving_string("", "", "") << std::endl;
    }
}


void Strings::TestAll()
{
    Strings::FindCommon_PrefixAndPostfix();
    Strings::FindLastNotOf__Benchmark();
    Strings::topKFrequent();
    Strings::PrintAllSubStrings();
    Strings::CountBinarySubstrings();
    Strings::Reverse_Words_in_String();
    Strings::Reverse_Words_in_String2();
    Strings::Compare_Version_Numbers();    // INFO: Unfinished yet | --> TODO
    Strings::Contains();
    Strings::Longest_Word();
    Strings::Intersperse_String();
    Strings::Count_Anagrams();
    Strings::Interleaving_String();
    Strings::DestinationCity();
}