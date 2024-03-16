/**============================================================================
Name        : Algorithms.cpp
Created on  : 11.06.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Algorithms
============================================================================**/

#include "Algorithms.h"
#include "../Performance/Performance.h"
#include "../Strings/StringUtilities.h"

#include <iostream>
#include <ranges>
#include <vector>
#include <optional>
#include <algorithm>
#include <numeric>

#include <array>
#include <numeric>
#include <cassert>
#include <cmath>
#include <cmath>
#include <set>
#include <map>
#include <memory>
#include <unordered_set>
#include <unordered_map>

namespace
{
    using StringPair = std::pair<std::string, std::string>;
    using IntPair = std::pair<int, int>;
}

namespace Algorithms
{
    template<typename _Ty>
    std::ostream& operator<<(std::ostream& stream, const std::vector<_Ty>& vec)
    {
        for (const auto & v: vec)
            stream << v << ' ';
        return stream;
    }

    template<typename _Ty>
    std::ostream& operator<<(std::ostream& stream, const std::set<_Ty>& vec)
    {
        for (const auto & v: vec)
            stream << v << ' ';
        return stream;
    }

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


namespace Algorithms::Numbers {
    std::vector<int> sortedSquaredArray(const std::vector<int> &numbers) {
        int right = 0;
        while (0 > numbers[right]) {
            ++right;
        }

        std::vector<int> result;
        result.reserve(numbers.size());

        const int size = numbers.size();
        int left = right - 1;
        while (left >= 0 && size > right) {
            const int leftVal = std::pow(numbers[left], 2), rightVal = std::pow(numbers[right], 2);
            if (leftVal > rightVal) {
                result.push_back(rightVal);
                ++right;
            } else {
                result.push_back(leftVal);
                --left;
            }
        }

        while (left >= 0)
            result.push_back(std::pow(numbers[left--], 2));
        while (size > right)
            result.push_back(std::pow(numbers[right++], 2));

        return result;
    }


    void printSortedSquaredNumber_InSortedArray() {

        std::vector<int> testData{-4, -3, -2, 0, 1, 5, 6, 9};
        std::vector<int> result = sortedSquaredArray(testData);

        for (int i: result)
            std::cout << i << ' ';
        std::cout << std::endl;
    }
}

namespace Algorithms::Numbers
{
    size_t _longest_increasing_subsequence_1(const std::vector<int> &Numeric) {
        // create an empty ordered set S. ith element in S is defined as the
        // smallest integer that ends an increasing sequence of length i
        std::set<int> tmp;

        for (int val: Numeric) {
            auto [iter, ok] = tmp.insert(val);
            // 1. If element IS NOT inserted at the END, then delete next greater element from set
            // 2. Ignore the current element if already present in the set
            if (ok && tmp.end() != std::next(iter))
                tmp.erase(std::next(iter));
        }
        return tmp.size();
    }

    /** Solution:
    The O(nlogn) solution is less obvious.

    If we consider building a subsequence iteratively, we have two situations.
    Either the next element is strictly higher, in which case we can simply append it.

    If it is not, we still want to remember it and we can do that by replacing the next higher value in the subsequence.

    This works because we only ever decrease values this way:
    - we won't break previous subsequences this way
    - we won't accidentally skip over potential subsequences
    **/

    size_t _longest_increasing_subsequence_2(const std::vector<int> &numbers) {
        std::vector<int> path;
        for (int64_t i = 0; i < std::ssize(numbers); ++i) {
            // Current element is higher than the tail of the path.
            if (path.empty() || path.back() < numbers[i]) {
                path.push_back(numbers[i]);
                continue;
            }

            // Find the element to overwrite
            auto it = std::upper_bound(path.begin(), path.end(), numbers[i], [](int l, int r) {
                return l <= r;
            });

            // it != path.end() because path.back() >= nums[i];
            *it = numbers[i];
        }
        return path.size();
    }

    void LongestIncreasingSubsequence() {
        std::vector<std::pair<std::vector<int>, size_t>> testData{
                {{1,  2,  3},                         3},
                {{3,  2,  1},                         1},
                {{2,  8,  4, 1,  9,  3,  5},          3},
                {{2,  1,  4, 3,  6,  5,  8,  7,  9},  5},
                {{1,  1,  1, 1},                      1},
                {{10, 22, 9, 33, 21, 50, 41, 60, 80}, 6},
                {{1,  2,  2, 3,  4,  4},              4}
        };
        for (const auto &[data, expectedResult]: testData) {
            std::cout << _longest_increasing_subsequence_1(data) << " | "
                      << _longest_increasing_subsequence_2(data) << " | "
                      << expectedResult << std::endl;
        }
    }
}

namespace Algorithms::Numbers {
    uint32_t find_rank(const std::vector<uint32_t> &papers) {
        std::map<uint32_t, uint32_t> tmp;
        for (uint64_t v: papers)
            ++tmp[v];

        uint32_t result = 0, count = 0;
        for (auto &[rank, ref_count]: std::ranges::reverse_view(tmp)) {
            count += ref_count;
            if (count >= rank)
                result = std::max(result, rank);
        }

        return result;
    }

    // Scientists are publishing papers, and these papers are getting cited in other papers.
    // Find the rank of the scientist.
    // Rank: largest R, such that at least R papers have >= R citations

    // std::vector<uint32_t> papers {3, 4, 5, 11}; ->  3
    // std::vector<uint32_t> papers {2, 4, 11};   ->  2

    /**
    Идея в том что бы структуры в виде вектора статей с количеством цитат (при ходит на вход)
    создать структуру данных (map)
    {
        [количество цитат] <--> [количество таких статей в векторе]
    }
    А далее итерируясь в обратном порядке
    подсчитывать значения в map-e --> считая колчество статей с данным и большым количеством цитат
    (что соответствует значению в map-e)
     */

    void Rank() {
        std::vector<uint32_t> papers{3, 4, 5, 11};

        int result = find_rank(papers);

        std::cout << result << std::endl;
    }
}

namespace Algorithms::Numbers
{
    bool contains_duplicate(const std::vector<int> &nums) {
        std::unordered_set<int> set(nums.begin(), nums.end());
        return nums.size() != set.size();
    }

    void Contains_Duplicate() {
        for (const std::vector<int> &values: std::vector<std::vector<int>>{
                {1, 2, 3, 4, 5},
                {1, 2, 3, 4, 5, 3}
        }) {
            std::cout << std::boolalpha << contains_duplicate(values) << std::endl;
        }
    }
}

namespace Algorithms::Numbers
{
    int find_duplicate(const std::vector<int> &values)
    {
        std::unordered_set<int> uniques;
        uniques.reserve(values.size());
        for (int v: values)
            if (!uniques.emplace(v).second)
                return v;
        return 0;
    }


    int find_duplicate_2(std::vector<int> &values)
    {
        while(values[0]!=values[values[0]])
            std::swap(values[0],values[values[0]]);
        return values.front();
    }

    void FindDuplicate()
    {
        for (std::vector<int> &values: std::vector<std::vector<int>>{
                {1,3,4,2,2}, // 2
                {3,1,3,4,2}, // 3
        }) {
            std::cout << find_duplicate(values) << "  " << find_duplicate_2(values) << std::endl;
        }
    }
}

namespace Algorithms::Numbers
{
    int missing_number(const std::vector<int>& values)
    {
        const int sum = std::accumulate(values.cbegin(), values.cend(), 0);
        return static_cast<int>((values.size()  * (values.size() + 1)) / 2) - sum;
    }

    /**
     * Given an array nums containing n distinct numbers in the range [0, n],
     * return the only number in the range that is missing from the array.
     * Example 1:
     *   nums = [3,0,1] -> 2
     *   nums = [0,1]   -> 2
     *   nums = [9,6,4,2,3,5,7,0,1]   -> 8
    */
    void MissingNumber()
    {
        for (const std::vector<int> &values: std::vector<std::vector<int>>{
                {3,0,1}, // 2
                {0,1},   // 2
                {9,6,4,2,3,5,7,0,1},   // 8
        }) {
            std::cout << missing_number(values) << std::endl;
        }
    }
}

namespace Algorithms::Numbers
{
    struct Stats final
    {
        int start {0};
        int   end {0};
        int count {0};

        explicit Stats(int start = 0, int end = 0, int count = 0):
                 start {start}, end {end}, count {count} { }
    };


    /**  Degree of an Array
     *
     * Given a non-empty array of non-negative integers nums, the degree of this array is defined
     * as the maximum frequency of any one of its elements.
     * Your task is to find the smallest possible length of a (contiguous) subarray of nums, that has the same degree as nums.
     *
     * Explanation:
     *   The input array has a degree of 2 because both elements 1 and 2 appear twice.
     *   Of the subarrays that have the same degree:  [1, 2, 2, 3, 1], [1, 2, 2, 3], [2, 2, 3, 1], [1, 2, 2], [2, 2, 3], [2, 2]
     *   The shortest length is 2. So return 2.
     *
     *  Explanation:
     *    The degree is 3 because the element 2 is repeated 3 times.
     *    So [2,2,3,1,4,2] is the shortest subarray, therefore returning 6.
     */
    int degreeOfArray(const std::vector<int>& values)
    {
        int maxOccurred = 0;
        std::unordered_map<int, Stats> counter {};
        counter.reserve(values.size()/2);
        for (int idx = 0, size = std::ssize(values); idx < size; ++idx)
        {
            const auto [iter, ok] = counter.emplace(values[idx], idx);
            ++(iter->second.count);
            iter->second.end = idx;
            maxOccurred = std::max(maxOccurred, iter->second.count);
        }

        int minLength = values.size();
        for (const auto &[key, stats]: counter)
        {
            if (stats.count == maxOccurred)
                minLength = std::min(minLength, stats.end - stats.start + 1);
        }

        return minLength;
    }

    void Degree_Of_Array()
    {
        for (const std::vector<int>& input: std::vector<std::vector<int>>{
                {1,2,2,3,1}, // -> 2
                {2,2,3,1,4,2}, // -> 6
                {2,1,1,2,1,3,3,3,1,3,1,3,2} // -> 7
        })
        {
            const auto result = degreeOfArray(input);
            std::cout << "Result = " << result << std::endl;
        }
    }
}

namespace Algorithms::Numbers
{
    std::vector<int> _top_K_Frequent(const std::vector<int>& nums, int k)
    {
        std::unordered_map<int, int> counter;
        for (int v: nums)
            ++counter[v];

        std::map<int, std::vector<int>> freqs;
        for (const auto [val, N]: counter)
            freqs[N].emplace_back(val);

        std::vector<int> out;
        out.reserve(k);
        for (auto iter = freqs.rbegin(); freqs.rend() != iter; ++iter) {
            std::sort(iter->second.begin(), iter->second.end());
            for (auto &&s: iter->second) {
                out.push_back(s);
                if (--k == 0)
                    return out;
            }
        }
        return out;
    }

    void topKFrequent()
    {
        for (const std::pair<std::vector<int>, int>& data: std::vector<std::pair<std::vector<int>, int>> {
                {{1,1,1,2,2,3}, 2} // ==> { 1,2 }
        })
        {
            std::cout << _top_K_Frequent(data.first, data.second) << std::endl;
        }
    }
}

namespace Algorithms::Strings
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

namespace Algorithms::Strings
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

namespace Algorithms::Numbers
{
    double median_of_two_sorted_arrays(const std::vector<int>& nums1,
                                       const std::vector<int>& nums2)
    {
        const int size1 = std::ssize(nums1), size2 = std::ssize(nums2);
        const int steps = (size1 + size2) / 2;
        int m1 = 0, m2 = 0;
        for (int n = 0, idx1 = 0, idx2 = 0; n <= steps; ++n)
        {
            m1 = m2;
            if (size1 > idx1 && size2 > idx2)
                m2 = nums1[idx1] > nums2[idx2] ? nums2[idx2++] : nums1[idx1++];
            else
                m2 = size1 > idx1 ? nums1[idx1++] : nums2[idx2++];
        }

        return (size1 + size2) % 2 == 1 ? static_cast<double>(m2) : static_cast<double>(m1 + m2) / 2;
    }

    /**
    Given two sorted arrays nums1 and nums2 of size m and n respectively, return the median of the two sorted arrays.
    The overall run time complexity should be O(log (m+n)).

    Example 1:    Input: nums1 = [1,3], nums2 = [2]  --> Result: 2.00000
                  Explanation: merged array = [1,2,3] and median is 2.

    Example 2:    Input: nums1 = [1,2], nums2 = [3,4] --> Result: 2.50000
                  Explanation: merged array = [1,2,3,4] and median is (2 + 3) / 2 = 2.5.
    */
    void Median_of_Two_Sorted_Arrays()
    {
        std::cout << median_of_two_sorted_arrays({1,3}, {2}) << std::endl;
        std::cout << median_of_two_sorted_arrays({1,2}, {3,4}) << std::endl;
    }
}

namespace Algorithms::Numbers
{
    int add_digits(int value)
    {
        int tmp = 0;
        while (value > 9) {
            tmp = value; value = 0;
            while (tmp > 9) {
                value += tmp % 10;
                tmp = tmp / 10;
            }
            value += tmp;
        }
        return value;
    }

    /**
    Given an integer num, repeatedly add all its digits until the result has only one digit, and return it.
    Example 1:  Input: num = 38     Explanation: The process is
                Output: 2           38 --> 3 + 8 --> 11
                                    11 --> 1 + 1 --> 2
                                    Since 2 has only one digit, return it.
    */
    void AddDigits()
    {
        int v = add_digits(10);
        std::cout << v << std::endl;
    }
}

namespace Algorithms::Numbers
{
    void rotate_array(std::vector<int>& nums, uint32_t k)
    {
        std::vector<int> result (nums.size(), 0);
        for (int i = 0, size = nums.size(); i < size; ++i)
            result[(k + i) % size] = nums[i];
        std::swap(nums, result);
    }

    void rotate_array_ex(std::vector<int>& nums, uint32_t k)
    {
        k = k % nums.size();
        if (nums.size() < 2 || 0 == k || k >= nums.size())
            return;
        std::vector<int> head (nums.end() - k, nums.end());
        for (uint32_t idx = nums.size() - 1; idx >= k; --idx)
            nums[idx] = nums[idx - k];
        for (uint32_t idx = 0; idx < k; ++idx)
            nums[idx] = head[idx];
    }

    void Rotate_Array()
    {
        std::vector<int> values {1, 2};
        rotate_array_ex(values, 1);

        std::cout << values << std::endl;
    }
}


namespace Algorithms::Numbers
{
    using namespace std::string_view_literals;

    std::string int_to_roman(int value)
    {
        constexpr std::array<std::pair<std::string_view, u_int16_t>, 14> numerals {{
            {"M"sv, 1000}, {"CM"sv, 900}, {"D"sv, 500}, {"CD"sv, 400},
            {"C"sv, 100}, {"XC"sv, 90}, {"LX"sv, 60}, {"L"sv, 50},
            {"XL"sv, 40}, {"X"sv, 10}, {"IX"sv, 9}, {"V"sv, 5}, {"IV"sv, 4}, {"I"sv, 1}
        }};

        std::string result;
        while (value) {
            for (const auto& [ch, val]: numerals) {
                if (value >= val) {
                    value -= val;
                    result.append(ch.data());
                    break;
                }
            }
        }
        return result;
    }

    /**
    Roman numerals are represented by seven different symbols: I, V, X, L, C, D and M.
        Symbol       Value
        I             1
        V             5
        X             10
        L             50
        C             100
        D             500
        M             1000

     For example, 2 is written as II in Roman numeral, just two one's added together.
     12 is written as XII, which is simply X + II. The number 27 is written as XXVII, which is XX + V + II.
    */
    void IntToRoman()
    {
        for (const auto  &[roman_number, expected]: std::vector<std::pair<int, std::string>> {
                {110, "CX"},
                {1650, "MDCL"},
                {1994, "MCMXCIV"},
        })
        {
            std::string actual = int_to_roman(roman_number);
            if (expected != actual) {
                std::cerr << expected << " != " << actual << std::endl;
                return;
            }
        }
        std::cout << "OK: All tests passed\n";
    }
}


namespace Algorithms::Numbers
{
    using namespace std::string_view_literals;

    int roman_to_int(const std::string& str)
    {
        int result = 0;
        for (size_t idx = 0, size = str.size(); idx < str.size(); ++idx)
        {
            const char c = str[idx];
            if ('M' == c)      result += 1000;
            else if ('D' == c) result += 500;
            else if ('C' == c) {
                if (size > idx && str[idx + 1] == 'D') {
                    result += 400;
                    ++idx;
                } else if (size > idx && str[idx + 1] == 'M') {
                    result += 900;
                    ++idx;
                } else {
                    result += 100;
                }
            }
            else if ('L' == c) result += 50;
            else if ('X' == c) {
                if (size > idx && str[idx + 1] == 'L') {
                    result += 40;
                    ++idx;
                } else if (size > idx && str[idx + 1] == 'C') {
                    result += 90;
                    ++idx;
                } else {
                    result += 10;
                }
            }
            else if ('V' == c) result += 5;
            else if ('I' == c) {
                if (size > idx && str[idx + 1] == 'V') {
                    result += 4;
                    ++idx;
                } else if (size > idx && str[idx + 1] == 'X') {
                    result += 9;
                    ++idx;
                } else {
                    result += 1;
                }
            }
        }
        return result;
    }


    void RomanToInt()
    {
        for (const auto  &[roman_num_str, expected]: std::vector<std::pair<std::string, int>> {
                { "LVIII", 58},
                { "MCMXCIV", 1994}
        })
        {
            const int actual = roman_to_int(roman_num_str);
            // std::cout << actual << std::endl;
            if (expected != actual) {
                std::cerr << expected << " != " << actual << std::endl;
                return;
            }
        }
        std::cout << "OK: All tests passed\n";
    }
}

namespace Algorithms::Numbers
{
    long divide(long dividend, long divisor)
    {
        const short s1 = dividend > 0 ? 1 : -1;
        const short s2 = divisor > 0 ? 1 : -1;

        dividend *= s1;
        divisor *= s2;

        if (1 == divisor)
            return s1 * s2 * dividend;

        int result = 0;
        while (dividend >= divisor) {
            dividend -= divisor;
            ++result;
        }
        return s1 * s2 * result;
    }

    void Divide_Numbers()
    {
        for (const auto  &[values, expected]: std::vector<std::pair<IntPair, int>> {
                {{1, 1}, 1}, {{1, -1}, -1}, {{-1, 1}, -1}, {{-1, -1}, 1},
                {{7, 2}, 3},
                {{7, -3}, -2},
                {{-2147483648, -1}, -44},
        }) {
            if (const auto actual = divide(values.first, values.second); expected != actual) {
                std::cerr << expected << " != " << actual << std::endl;
                return;
            }
        }
        std::cout << "OK: All tests passed\n";
    }
}


namespace Algorithms::Bits
{
    template<typename T>
    void show_bits(T x)
    {
        std::cout << x << "  ===>  ";
        for (int i = (sizeof(T) * 8) - 1; i >= 0; i--)
            std::cout << (x & (1u << i) ? '1' : '0');
        std::cout << std::endl;
    }

    /**
    Reverse bits of a given 32 bits unsigned integer.
    Input: n = 00000010100101000001111010011100
    Output:    964176192 (00111001011110000010100101000000)
    Input: n = 11111111111111111111111111111101
    Output:   3221225471 (10111111111111111111111111111111)
    **/

    uint32_t reverseBits(uint32_t input)
    {
        uint32_t output = 0;
        for (int i = (sizeof(uint32_t) * 8) - 1, n = 0; i >= 0; --i, ++n)
        {
            if ((input & (1u << i))) {
                output |= (1 << n);
            }
        }
        return output;
    }

    void ReverseBits()
    {
        uint32_t val = reverseBits(5);
        show_bits(5);
        show_bits(val);
    }
}

namespace Algorithms::Strings
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

namespace Algorithms::Strings
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

namespace Algorithms::Strings
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

namespace Algorithms::Strings
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

namespace Algorithms::Numbers
{
    int find_max_three_numbers_product(const std::vector<int>& values)
    {
        int maxProduct = values[0] * values[1] * values[2];
        const int size = values.size();
        for (int i = 0; i < size; ++i)
        {
            for (int m = 0; m < size; ++m)
            {
                for (int n = 0; n < size; ++n)
                {
                    if (n == m || i == m || i == n)
                        continue;
                    maxProduct = std::max( maxProduct, values[i] * values[m] * values[n]);
                }
            }
        }
        return maxProduct;
    }

    int find_max_three_numbers_product_2(std::vector<int>& values)
    {
        std::sort(values.begin(), values.end(), std::greater<int>{});
        return values[0] * values[1] * values[2];
    }

    void MaximumProduct_of_ThreeNumbers()
    {
        for (std::vector<int>& v: std::vector<std::vector<int>> {
                {1, 2, 3},
                {1, 2, 3, 4},
                {-1, -2, -3},
                {-1, -2, -3, -4, -5, 6},
        })
        {
            std::cout << find_max_three_numbers_product(v) <<  " "
                      << find_max_three_numbers_product_2(v) << std::endl;
        }
    }
}

namespace Algorithms::Majority
{
    /** Problem:
    Given an array of integers as std::vector<int>, return
    the majority element (guaranteed to be present).

    A majority element has more than size/2 number of instances in the array.

    There is a solution that runs in O(n) time and O(1) space.
    **/

    int majority_element(const std::vector<int>& nums)
    {
        int major = 0;
        for (size_t count = 0; auto v : nums) {
            if (0 == count) {
                major = v;
                count = 1;
            } else if (v == major) {
                ++count;
            } else {
                --count;
            }
        }
        return major;
    }

    void Test()
    {
        for (const std::vector<int>& v: std::vector<std::vector<int>> {
                // {0},
                // {1,1,1,2},
                {2,2,1,1,1},
                // {1,0,1,2,1,3,1,5},
                // {1,2,3,4,5,6,6,6,6,6,6,6},
        })
        {
            const int major = majority_element(v);
            std::cout << "[ " << v << "] ==> " << major << std::endl;
        }
    }
}

namespace MaxTree
{
    /** Problem:
    Given an array of unique integers, construct a binary max-tree.

    A max tree is constructed by picking the maximum element as the root;
    all elements to the left of the maximum belong to the left child subtree, and all elements to the right belong to the right subtree.
    Both subtrees recursively follow the same logic.

    Solution:
    Let's consider traversing elements left-to-right.

    If the element is lower than the previous one, we can add it as right child.

    If it isn't we need to find the correct place, which will be as the right child of the next higher node
    and the entire subtree of lower values will become the left child of the inserted node.

    We can achieve this using a monotonic stack.
    **/

    struct Node
    {
        int value { 0 };
        Node* left { nullptr };
        Node* right { nullptr };
    };

    struct Tree
    {
        Node* add(int value) {
            return store.emplace_back(std::make_unique<Node>(value, nullptr, nullptr)).get();
            // return store.back().get();
        }

        Node* root = nullptr;

    private:
        std::vector<std::unique_ptr<Node>> store;
    };


    Tree make_max_tree(const std::vector<int>& nums)
    {
        Tree t;
        std::vector<Node*> s;
        for (int v : nums)
        {
            Node* node = t.add(v);

            // Until we find the first higher value.
            while (!s.empty() && s.back()->value < v) {
                // Once we stop we have node->left pointing to the root of the tree representing lower values.
                node->left = s.back();
                s.pop_back();
            }

            // If there is a higher value we have seen this node belongs as the right child.
            if (!s.empty())
                s.back()->right = node;

            // Continue to the next element.
            s.push_back(node);
        }

        t.root = s.front();
        return t;
    }

    // template<typename T>
    void validate(bool condition)
    {
        if (!condition)
        {
            std::cout << "ERROR\n";
        }
    }

    void test()
    {
        Tree t4 = make_max_tree({6,7,5,9,8,4});

        validate(t4.root->value == 9);
        validate(t4.root->left->value == 7);
        validate(t4.root->left->left->value == 6);
        validate(t4.root->left->right->value == 5);
        validate(t4.root->right->value == 8);
        validate(t4.root->right->right->value == 4);
    }
}

namespace Algorithms::Strings
{
    using Performance::Utils::ScopedTimer;

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


namespace Algorithms::Strings
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

namespace Algorithms::Strings
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

namespace Algorithms::Strings
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

    void Longest_Word()
    {
        for (const auto  &[value, expected]: std::vector<StringPair> {
                {"11 222 33", "222"},
                {"1", "1"},
                {"", ""},
                {"Hello world aaaaaa bbbbbb", "aaaaaa"},

        })
        {
            const std::string actual = longest_word(value);
            if (expected != actual) {
                std::cerr << expected << " != " << actual << std::endl;
                return;
            }
        }
        std::cout << "OK: All tests passed\n";
    }
}

namespace Algorithms::Strings
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


namespace Algorithms::Strings
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


namespace Algorithms::Strings
{
    bool is_interleaving_string(const std::string& str1,
                                const std::string& str2,
                                const std::string& dest)
    {
        for (size_t idx = 0, i = 0, n = 0; idx < dest.size(); ++idx)
        {
            if (str1[i] != dest[idx] && str2[n] != dest[idx])
                return false;
            else if (str1.size() > i && str1[i] == dest[idx])
                ++i;
            else if (str2.size() > n && str2[n] == dest[idx])
                ++n;

            std::cout << idx << "  "  << i << " " << n << " " << dest[idx] << std::endl;
        }
        return true;
    }

    void Interleaving_String()
    {

        std::cout << is_interleaving_string("aabcc", "dbbca", "aadbbcbcac") << std::endl;
    }
}

void Algorithms::TestAll()
{
    // Algorithms::Devide_SubArray();
    // Algorithms::BinarySearch();
    // Algorithms::PrintAllSubArraysTest();

    // Numbers::printSortedSquaredNumber_InSortedArray();
    // Numbers::LongestIncreasingSubsequence();
    // Numbers::Contains_Duplicate();
    // Numbers::FindDuplicate();
    // Numbers::MissingNumber();
    // Numbers::Rank();
    // Numbers::Degree_Of_Array();
    // Numbers::topKFrequent();
    // Numbers::Median_of_Two_Sorted_Arrays();
    // Numbers::AddDigits();
    // Numbers::Rotate_Array();
    // Numbers::IntToRoman();
    // Numbers::RomanToInt();
    // Numbers::Divide_Numbers();

    // Strings::FindCommon_PrefixAndPostfix();
    // Strings::FindLastNotOf__Benchmark();
    // Strings::topKFrequent();
    // Strings::PrintAllSubStrings();
    // Strings::CountBinarySubstrings();
    // Strings::Reverse_Words_in_String();
    // Strings::Reverse_Words_in_String2();
    // Strings::Compare_Version_Numbers();    // INFO: Unfinished yet | --> TODO
    // Strings::Contains();
    // Strings::Longest_Word();
    // Strings::Intersperse_String();
    // Strings::Count_Anagrams();
    Strings::Interleaving_String();

    // Bits::ReverseBits();

    // MaxTree::test();

    // Majority::Test();

    // Algorithms::Sqrt();

    // CalcParentheses_3_BRacket();

    // smallest_substring_containing_all_chars();

    // BoundedSubArrays::Tests();

    // FindCommonElements_3_SortedArrays();
};

