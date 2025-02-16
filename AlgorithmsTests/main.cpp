/**============================================================================
Name        : AlgoritmsTests.cpp
Created on  : 14.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "Strings/Strings.h"


#include <iostream>
#include <string_view>
#include <set>
#include <map>
#include <list>
#include <span>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <array>
#include <numeric>
#include <memory>
#include <cmath>
#include <ranges>
#include <iomanip>


namespace
{
    using StringPair = std::pair<std::string, std::string>;
    using IntPair = std::pair<int, int>;
}

namespace
{
    template<typename T>
    std::ostream &operator<<(std::ostream &stream, const std::vector<T> &list)
    {
        for (const auto &i: list)
            stream << " " << i;
        return stream;
    }

    template<typename T>
    std::ostream &operator<<(std::ostream &stream, const std::list<T> &list)
    {
        for (const auto &i: list)
            stream << " " << i;
        return stream;
    }

    template<typename T>
    std::ostream &operator<<(std::ostream &stream, const std::set<T> &set)
    {
        for (const auto &i: set)
            stream << " " << i;
        return stream;
    }

    template<typename T>
    std::ostream &operator<<(std::ostream &stream, const std::unordered_set<T> &set)
    {
        for (const auto &i: set)
            stream << " " << i;
        return stream;
    }

    template<typename T>
    std::ostream &operator<<(std::ostream &stream, const std::deque<T> &list)
    {
        for (const auto &i: list)
            stream << " " << i;
        return stream;
    }
}

namespace Algorithms
{
    std::vector<std::vector<int>> parts {};

    void addPiece(const std::vector<int>& vect, size_t start, size_t end)
    {
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


    void Divide_SubArray()
    {
        std::vector<int> origin {1, 2, 3, 4};

        splitToPieces(origin);

        for (const auto& p: parts) {
            std::cout << "[ ";
            for (const auto i: p)
                std::cout << i << " ";
            std::cout << "]";
        }
    }
}

namespace Algorithms
{
    template<typename T>
    std::optional<T> binary_search(const std::vector<T>& numbers,
                                   const T value)
    {
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
}

namespace Algorithms
{
    int _sqrt(int x)
    {
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

    void Sqrt()
    {
        int s = _sqrt(9);
        std::cout << s << std::endl;
    }

}


namespace Algorithms::BoundedSubArrays
{
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

namespace Algorithms::Numbers
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
        for (uint32_t count = 0; const int value : nums) {
            if (0 == count) {
                major = value;
                count = 1;
            } else if (value == major) {
                ++count;
            } else {
                --count;
            }
        }
        return major;
    }

    void MajorityElement()
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

namespace Algorithms::Numbers
{


    bool can_partition(const std::vector<int>& nums)
    {
        // Calculate the sum of elements in the nums array
        int totalSum = accumulate(nums.begin(), nums.end(), 0);

        // If the total sum is odd, it's not possible to divide it into two equal parts
        if (totalSum % 2 == 1) {
            return false;
        }

        // Target sum for each partition
        int targetSum = totalSum >> 1;

        // Create a dynamic programming array to keep track of possible sums
        std::vector<bool> dp(targetSum + 1,false);

        // The sum of 0 is always achievable (by selecting no elements)
        dp[0] = true;

        // Iterate through the numbers in the array
        for (int num : nums) {
            // Check each possible sum in reverse to avoid using a number twice
            for (int j = targetSum; j >= num; --j) {
                // Update the dp array: dp[j] will be true if dp[j - num] was true
                // This means that current number 'num' can add up to 'j' using the previous numbers
                dp[j] = dp[j] || dp[j - num];
            }
        }

        // The result is whether it's possible to achieve the targetSum using the array elements
        return dp[targetSum];
    }

    void CanPartition()
    {
        for (const auto & [values, expected]: std::vector<std::pair<std::vector<int>, bool>> {
                {{0}, true},
                {{2,5,12,5}, true},
                {{1,2,3,5}, false},
                {{2, 2, 2, 3, 3}, true},
        })
        {
            const bool actual = can_partition(values);
            if (expected != actual) {
                std::cerr << expected << " != " << actual << std::endl;
                return;
            }
        }
        std::cout << "OK: All tests passed\n";
    }
}

namespace PairSum
{
    std::pair<int, int> twoSum(const std::vector<int>& numbers, int target)
    {
        for (int l = 0, r = numbers.size() - 1; l < r;)
        {
            if (numbers[l] + numbers[r] == target) {
                return {l, r};
            }
            if (numbers[l] + numbers[r] < target) {
                ++l;
            } else {
                --r;
            }
        }
        return {-1, -1};
    }

    std::pair<int32_t, int32_t> twoSum_Values(const std::vector<int32_t>& values, const int32_t K)
    {
        for (int32_t idxLeft = 0, idxRight = values.size() - 1; idxLeft < idxRight;) {
            if (values[idxLeft] + values[idxRight] == K) {
                return { values[idxLeft], values[idxRight] };
            }
            if (values[idxLeft] + values[idxRight] < K) {
                ++idxLeft;
            } else {
                --idxRight;
            }
        }
        return {-1 , -1};
    }

    void TestTwoSum()
    {
        {
            const std::vector<int> values = { 2, 4, 5, 6, 7, 8, 9, 11 };
            const std::pair<int, int> result = twoSum(values, 9);
            std::cout << values[result.first] << ", " <<  values[result.second] << std::endl;
        }

        {
            const std::vector<int> values = { 2, 4, 5, 6, 7, 8, 9, 11 };
            const std::pair<int, int> result = twoSum_Values(values, 9);
            std::cout << result.first << ", " << result.second << std::endl;
        }
    }
}


namespace Algorithms::Experiments
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


int main([[maybe_unused]] int argc,
		 [[maybe_unused]] char** argv)
{
    using namespace Algorithms;
    // Strings::TestAll();

    /*
    Algorithms::Divide_SubArray();
    Algorithms::BinarySearch();
    Algorithms::Sqrt();
    Algorithms::FindCommonElements_3_SortedArrays();
    // Algorithms::PrintAllSubArraysTest();

    Numbers::printSortedSquaredNumber_InSortedArray();
    Numbers::LongestIncreasingSubsequence();
    Numbers::MissingNumber();
    Numbers::Rank();
    Numbers::Degree_Of_Array();
    Numbers::topKFrequent();
    Numbers::Median_of_Two_Sorted_Arrays();
    Numbers::AddDigits();
    Numbers::Rotate_Array();
    Numbers::IntToRoman();
    Numbers::RomanToInt();
    Numbers::Divide_Numbers();
    Numbers::MajorityElement();              // INFO --> To Algorithms
    Numbers::CanPartition();                 // INFO --> To Algorithms

    Experiments::CalcParentheses_3_BRacket();
     */


	return EXIT_SUCCESS;
}
