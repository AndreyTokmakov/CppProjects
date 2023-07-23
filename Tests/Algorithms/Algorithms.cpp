/**============================================================================
Name        : Algorithms.cpp
Created on  : 11.06.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Algorithms
============================================================================**/

#include "Algorithms.h"

#include <iostream>
#include <vector>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <array>
#include <numeric>
#include <cassert>

namespace Algorithms
{
    void printArray(const std::vector<int>& vect, size_t start, size_t end) {
        while (end > start)
            std::cout << vect[start++] << " ";
        std::cout << std::endl;
    }

    void printAllSubArrays(const std::vector<int>& vect)
    {
        for (size_t size = vect.size(), i = 0; i < size; ++i) {
            for (size_t n = size; n > i; n--)
                printArray(vect, i, n);
            std::cout << "-------------" << std::endl;
        }
    }

    void PrintAllSubArraysTest() {
        printAllSubArrays({1,2,3,4});
    }

    //------------------------------------------------------------------------------

    template<typename T>
    std::optional<T> binary_search(const std::vector<T>& numbers,
                                   const T value) {
        size_t left = 0, right = numbers.size() - 1, idxMid = 0;
        while (right > (left + 1)) {
            idxMid = (left + right) / 2;
            if (value == numbers[idxMid])
                return numbers[idxMid];
            if (numbers[idxMid] > value)
                right = idxMid;
            else
                left = idxMid;
        }
        return std::nullopt;
    }

    void BinarySearch()
    {

        std::vector<int> numbers {1,2,3,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

        const int v = 13;
        const auto result = binary_search(numbers, v);

        if (result.has_value())
            std::cout << result.value() << std::endl;
        else
            std::cout << "Failed to find!\n";
    }

    //------------------------------------------------------------------------------


    std::vector<std::vector<int>> parts {};

    void addPiece(const std::vector<int>& vect, size_t start, size_t end) {
        // if (end == start + 1) return;
        std::vector<int>& p = parts.emplace_back();
        while (end > start)
            p.push_back(vect[start++]);
    }

    void splitToPieces(const std::vector<int>& vect) {
        for (size_t size = vect.size(), i = 0; i < size; ++i) {
            for (size_t n = size; n > i; n--)
                addPiece(vect, i, n);
        }
    }

    void Devide_SubArray() {
        std::vector<int> origin {1, 2, 3, 4};

        splitToPieces(origin);

        for (const auto& p: parts) {
            std::cout << "[ ";
            for (const auto i: p)
                std::cout << i << " ";
            std::cout << "]";
        }
    }

    //------------------------------------------------------------------------------------

    int _sqrt(int x) {
        int left = 0, right = x, middle = 0, result = 0;
        while (left <= right) {
            middle = (right + left) / 2;
            result = middle * middle;
            if (result == x)
                return middle;
            else if (result > x)
                right = middle - 1;
            else
                left = middle + 1;
        }
        return right;
    }

    void Sqrt() {
        int s = _sqrt(9);
        std::cout << s << std::endl;
    }

    //------------------------------------------------------------------------------------

    bool __contains(const std::string& text,
                    const std::string& str)
    {
        for (size_t idx = 0, m = 0, n= 0; idx < text.size() - str.size(); ++idx)
        {
            for (m = 0, n = idx; m < str.size(); ++m, ++n) {
                if (str[m] != text[n])
                    break;
            }
            if (m == str.size())
                return true;
        }
        return false;
    }

    void Contains() {
        std::string text = "aaaaaabbaaaaaa", str = "bbb";

        std::cout << std::boolalpha << __contains(text, str) << std::endl;

    }

    /*
    'A harbor manager has a port with (n) ships. Each ship has (m) units of cargo on them already.
    In order for the ships to leave the harbor they need to all carry an equal number of cargo units.
    Given inputs (vector<int> &ships, int n) print the least amount of moves it takes to make all ships contain equal
    amounts of cargo. Print -1 if it’s not possible'
    */
    int calcMinMoves(const std::vector<int> &ships, [[maybe_unused]] int n)
    {
        const int unitsTotal = std::accumulate(ships.cbegin(), ships.cend(), 0);
        if (unitsTotal % 2)
            return -1;

        const int avg = unitsTotal / ships.size();
        return std::accumulate(ships.cbegin(), ships.cend(), 0, [avg](auto res, auto val) {
            return res + std::abs(avg - val);
        }) / 2;
    }

    //--------------------------------------------------------------------------------------

    bool __check_parentheses_3_bracket(const std::string& str) {
        std::vector<char> brackets;
        brackets.reserve(str.length());
        for (const char c : str) {
            if ('(' == c || '[' == c || '{' == c) {
                brackets.push_back(c);
            }
            else if (')' == c || ']' == c || '}' == c) {
                if (brackets.empty()) {
                    return false;
                }
                else if (')' == c) {
                    if ('(' == brackets.back())
                        brackets.pop_back();
                    else
                        return false;
                }
                else if ('}' == c) {
                    if ('{' == brackets.back())
                        brackets.pop_back();
                    else
                        return false;
                }
                else if (']' == c) {
                    if ('[' == brackets.back())
                        brackets.pop_back();
                    else
                        return false;
                }
                else {
                    return false;
                }
            }
        }
        return brackets.empty();
    }

    bool check_parentheses_3_bracket_ex(const std::string& str)
    {
        constexpr std::array<std::pair<char, char>, 3> symbols {{
            {'[', ']'}, {'{', '}'}, {'(', ')'}
        }};
        std::vector<char> brackets;
        for (const char c : str) {
            for (const auto& [open, close]: symbols) {
                if (c == open)
                    brackets.push_back(c);
                else if (c == close) {
                    if (brackets.empty() || open != brackets.back())
                        return false;
                    else
                        brackets.pop_back();
                }
            }
        }
        return brackets.empty();
    }

    void CalcParentheses_3_BRacket() {

        for (const auto& str: {"{[(]}", "()", "()()", "({}[{}])"})
        {
            std::cout << str << " = " << std::boolalpha << __check_parentheses_3_bracket(str) << std::endl;
            std::cout << str << " = " << std::boolalpha << check_parentheses_3_bracket_ex(str) << std::endl;
        }
    }


    /** ======================================================================================= **/

    // Smallest window in a String containing all characters of other String
    // Given two strings, string and pattern, the task is to find the smallest substring in string
    // containing all characters of pattern.

    void smallest_substring_containing_all_chars()
    {
        const std::string str = "aaaa this is a test string",  pattern = "tist";
        const std::array<int16_t, 256> map = [&pattern]()
        {
            std::array<int16_t, 256> tmp {};
            for (char c: pattern)
                ++tmp[c];
            return tmp;
        }();

        size_t left = 0;
        for (; left < str.size()  && 0 == map[str[left]]; ++left) { }

        std::array<int16_t, 256> tmp (map);
        size_t right = left;
        for (size_t count = pattern.length(); right < str.size(); ++right) {
            if (tmp[str[right]]-- > 0)
                if (0 == --count)
                    break;
        }



        for (size_t idx = left; idx <= right; ++idx)
            std::cout << str[idx];
        std::cout << std::endl;
    }
}

namespace Algorithms::BoundedSubArrays {
    // Given an array of integers and two boundary values, A and B, return the number of (contiguous) subarrays
    // for which the value A is the minimum and value B is the maximum of the subarray.

    /* Solution:
    If we only have values from the range [A, B], we can calculate
    the number of subarrays ending in each index.

    For example A==2,B==4: {3,3,2,4} the number of instances is three,
    which is std::min(a_idx,b_idx)+1: {3,3,2,4},{3,2,4},{2,4}.

    If we add another A, we can the also omit the previous instace of A:
    {3,3,2,4,2}, the previous three instances + four new instances:
    {3,3,2,4,2},{3,2,4,2},{2,4,2},{4,2}, again std::min(a_idx,b_idx)+1.

    The only complication are values outside of the range [A,B].

    However, these values simply create a boundary, meaning that instead
    of calculating the number of subarrays against the start of the array
    we are now calculating the number against the last boundary.
    */

    int64_t boundedSubArrays(const std::vector<int64_t> &nums,
                             const int64_t min, const int64_t max) {
        int64_t count = 0, rightmostMin = -1, rightmostMax = -1, rightmostBlocker = -1;

        for (int64_t i = 0; i < std::ssize(nums); ++i) {
            // If we observe a value outside of the range, move the left boundary.
            if (nums[i] < min || nums[i] > max) {
                rightmostBlocker = i;
                continue; // optional
            }

            // It is possible that min == max
            if (nums[i] == min)
                rightmostMin = i;
            if (nums[i] == max)
                rightmostMax = i;

            // Implicit + 1 because we are deducting an index.
            int64_t subarrays = std::min(rightmostMin, rightmostMax)
                                - rightmostBlocker;
            // Instead of resetting the rightmostMin and rightmostMax when we see a boundary we simply rely
            // on the fact that subarrays will end up negative if min < rightmostBlocker.
            count += std::max(int64_t{0}, subarrays);
        }
        return count;
    }

    size_t boundedSubArraysEx(const std::vector<int64_t> &values,
                              const int64_t min, const int64_t max) {

        size_t count{0}, len{0};
        bool hasMix = false, hasMax = false;
        for (const auto &value: values) {
            if (max >= value && value >= min) {
                ++len;
                if (min == value) hasMix = true;
                if (max == value) hasMax = true;
                if (hasMix && hasMax)
                    count += len - 1;


                std::cout << value << " | " << std::boolalpha << (bool) (hasMix && hasMax)
                          << " | " << len << " | " << count << std::endl;

            } else {
                len = 0;
                hasMix = hasMax = false;
            }
        }
        return count;
    }

    // UNFINISHED
    size_t boundedSubArrays2(const std::vector<int64_t> &values,
                             const int64_t min, const int64_t max) {

        size_t count{0}, minCount{0}, maxCount{0}, start = 0;
        for (size_t idx = 0; idx < values.size(); ++idx) {
            if (const auto value = values[idx]; max >= value && value >= min) {
                if (min == value)
                    ++minCount;
                if (max == value)
                    ++maxCount;
            } else {
                if (minCount && maxCount) {
                    size_t left = start, right = idx;
                    while (minCount && maxCount) {
                        if (min == values[left++]) {
                            ++count;
                            //++left;
                            --minCount;
                        }
                        if (max == values[right]) {
                            ++count;
                            --right;
                            --maxCount;
                        }
                    }
                }

                maxCount = minCount = 0;
                start = idx + 1;
            }
        }

        if (minCount && maxCount) {
            for (size_t i = start; i < values.size(); ++i)
                std::cout << values[i] << " ";
            std::cout << std::endl;
        }

        return count;
    }


    void Tests() {
        const std::vector<std::pair<std::vector<int64_t>, std::pair<int64_t, int64_t>>> testDatasource
                {
                        // {{2,3,3,2,4}, {2, 4}},  // --> 4
                        //{{3,3,2,4,4}, {2, 4}},  // --> 7
                        // {{3,3,2,4,2,3}, {2, 4}},  // --> 7
                        {{3, 2, 4, 1, 2, 3, 4}, {2, 4}},
                };

        for (const auto &testData: testDatasource) {
            std::cout
                    // << boundedSubArrays(testData.first,testData.second.first, testData.second.second)
                    // << " - "
                    // << boundedSubArraysEx(testData.first,testData.second.first, testData.second.second)
                    << boundedSubArrays2(testData.first, testData.second.first, testData.second.second)
                    << std::endl;
        }
    }
}

namespace Algorithms
{

    void findCommon(const std::vector<int>& ar1,
                    const std::vector<int>& ar2,
                    const std::vector<int>& ar3)
    {
        for (size_t i = 0, j = 0, k = 0; i < ar1.size() && j < ar2.size()  && k < ar3.size(); /** **/)
        {
            if (ar1[i] == ar2[j] && ar2[j] == ar3[k]) { // If x = y and y = z, print any of them and move ahead in all arrays
                std::cout << ar1[i] << " ";
                ++i; ++j; ++k;
            }

            else if (ar1[i] < ar2[j])  // x < y
                i++;
            else if (ar2[j] < ar3[k])  // y < z
                j++;
            else                       // We reach here when x > y and z < y, i.e., z is smallest
                k++;
        }
    }


    // Find common elements in three sorted arrays
    void FindCommonElements_3_SortedArrays()
    {
        std::vector<int> array1 { 1, 5, 10, 20, 40, 80 };
        std::vector<int> array2 { 6, 7, 20, 80, 100 };
        std::vector<int> array3 { 3, 4, 15, 20, 30, 70, 80, 120 };

        std::cout << "Common Elements are ";
        findCommon(array1, array2, array3);
    }
}

void Algorithms::TestAll()
{
    // Algorithms::Devide_SubArray();
    // Algorithms::BinarySearch();
    // Algorithms::PrintAllSubArraysTest();

    // Algorithms::Sqrt();sdsd

    // Contains();

    // CalcParentheses_3_BRacket();

    // smallest_substring_containing_all_chars();

    // BoundedSubArrays::Tests();

    FindCommonElements_3_SortedArrays();
};

