//============================================================================
// Name        : Algorithms.cpp
// Created on  : 2021-11-06.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Algorithms
//============================================================================

#include "Algorithms.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <string_view>
#include <array>
#include <functional>
#include <algorithm>
#include <ctime>
#include <cctype>
#include <numeric>
#include <list>
#include <vector>
#include <iterator>
#include <chrono>
#include <memory>
#include <thread>
#include <unordered_set>
#include <random>
#include <execution>
#include "../Integer/Integer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												      Algorithms_Tests :										     	    	                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Algorithms {

    class Summ {
    public:
        int sum;

    public:
        Summ() : sum{ 0 } {
        }
        void operator()(int n) {
            sum += n;
        }
    };

    int op_increase(int i) {
        return ++i;
    }

    auto printer_coma = [](const auto& val) {
        std::cout << val << ", ";
    };

    auto print_vector = [](const auto& vector) {
        std::for_each(vector.begin(), vector.end(), printer_coma);
    };

    auto print_vector_ex = [](const auto& vector,
                              std::string_view text = "",
                              std::string_view postfix = "\n") {
        std::cout << text;
        print_vector(vector);
        std::cout << postfix << std::endl;
    };


    template<class Iter>
    void print_range(Iter first, Iter last, std::string_view text, std::string_view postfix = "\n") {
        std::cout << text;
        std::for_each(first, last, printer_coma);
        std::cout << postfix;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& ostr, const std::list<T>& list) {
        for (const auto& i : list)
            ostr << " " << i;
        return ostr;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& ostr, const std::vector<T>& list) {
        for (const auto& i : list)
            ostr << i << ' ';
        return ostr;
    }

    template<typename T, size_t Size>
    std::ostream& operator<<(std::ostream& ostr, const std::array<T, Size>& array) {
        for (const auto& i : array)
            ostr << i << ' ';
        return ostr;
    }

    class A {};

    void IsArithmetic() {
        std::cout << std::boolalpha;
        std::cout << "A:           " << std::is_arithmetic<A>::value << '\n';
        std::cout << "int:         " << std::is_arithmetic<int>::value << '\n';
        std::cout << "int const:   " << std::is_arithmetic<int const>::value << '\n';
        std::cout << "int &:       " << std::is_arithmetic<int&>::value << '\n';
        std::cout << "int *:       " << std::is_arithmetic<int*>::value << '\n';
        std::cout << "float:       " << std::is_arithmetic<float>::value << '\n';
        std::cout << "float const: " << std::is_arithmetic<float const>::value << '\n';
        std::cout << "float &:     " << std::is_arithmetic<float&>::value << '\n';
        std::cout << "float *:     " << std::is_arithmetic<float*>::value << '\n';

        std::cout << "Integer:     " << std::is_arithmetic<Integer>::value << '\n';
    }

    void Distance()
    {
        {
            std::vector<int> vect = { 3, 1, 4 };
            auto distance = std::distance(vect.begin(), vect.end());
            std::cout << "distance = " << distance << std::endl;
        }

        {
            std::list<int> mylist;
            for (int i = 0; i < 10; i++)
                mylist.push_back(i * 10);

            std::list<int>::iterator first = mylist.begin();
            std::list<int>::iterator last = mylist.end();

            std::cout << "The distance[first, last] is: " << std::distance(first, last) << std::endl;
            std::cout << "The distance[++first, --last] is: " << std::distance(++first, --last) << std::endl;
        }
    }

    void Uninitialized_Copy() {
        std::string numbers[] = { "one","two","three" };

#if 0
        // get_temporary_buffer deprecated in C++20
		std::pair <std::string*, std::ptrdiff_t> result = std::get_temporary_buffer<std::string>(3);
		if (result.second > 0) {
			std::uninitialized_copy(numbers, numbers + result.second, result.first);

			for (int i = 0; i < result.second; i++)
				std::cout << result.first[i] << " ";
			std::cout << '\n';

			std::return_temporary_buffer(result.first);
		}
#endif
    }

    void Partition() {
        std::cout << "--------------------------------- Test1: ----------------------------------" << std::endl;
        {
            const auto  isOdd = [](int i)->bool { return (i % 2) == 1; };
            std::vector<int> vect = { 1,2,3,4,5,6,7,8,9 };

            auto bound = std::partition(vect.begin(), vect.end(), isOdd);
            print_range(vect.begin(), bound, "odd elements : ");
            print_range(bound, vect.end(), "even elements : ");
        }
        std::cout << "\n--------------------------------- Test2: ----------------------------------" << std::endl;
        {
            const auto greatern_than_6 = [](int i)->bool { return i > 6; };
            std::vector<int> vect = { 1,6,2,8,9,4,2,7 };

            auto bound = std::partition(vect.begin(), vect.end(), greatern_than_6);
            print_range(vect.begin(), bound, "> 6  : ");
            print_range(bound, vect.end(), "< 6  : ");
        }
    }

    void Next_Permutation() {
        std::string s = "123";
        std::sort(s.begin(), s.end());
        do {
            std::cout << s << std::endl;
        } while (std::next_permutation(s.begin(), s.end()));
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												      NonModifying :		       								     	    	                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Algorithms::NonModifying {

    void Find() {
        int n1 = 3, n2 = 5;
        const std::vector<int> ints = { 0, 1, 2, 3, 4 };

        auto test_find = []<typename T>(const std::vector<T>& data, const T value) {
            auto iter = std::find(data.cbegin(), data.cend(), value);
            if (iter != data.end()) {
                std::cout << "'ints' contains " << *iter << std::endl;
            }
            else {
                std::cout << "'ints' do NOT contains " << value << std::endl;
            }

        };

        test_find(ints, n1);
        test_find(ints, n2);
    }

    void Find_IF() {
        const std::vector<int> ints = { 1,2,3,4,5 };

        if (auto iter = std::find_if(ints.begin(), ints.end(), [](int v) {return ((v % 2) == 0); }); iter != ints.end()) {
            std::cout << "The first EVEN value is " << *iter << std::endl;
        }
        else {
            std::cout << "Failed to find EVEN value" << std::endl;
        }

        if (auto iter = std::find_if(ints.begin(), ints.end(), [](int v) {return ((v % 2) == 1); }); iter != ints.end()) {
            std::cout << "The first ODD is " << *iter << std::endl;
        }
        else {
            std::cout << "Failed to find ODD value" << std::endl;
        }
    }

    void Find_IF_Not() {
        const std::vector<int> ints = { 1,2,3,4,5 };

        if (auto iter = std::find_if_not(ints.begin(), ints.end(), [](int v) {return ((v % 2) == 1); }); iter != ints.end()) {
            std::cout << "The first NOT ODD is " << *iter << std::endl;
        } else {
            std::cout << "Failed to find NOT ODD value" << std::endl;
        }

        if (auto iter = std::find_if(ints.begin(), ints.end(), [](int v) {return ((v % 2) == 0); }); iter != ints.end()) {
            std::cout << "The first EVEN value is " << *iter << std::endl;
        } else {
            std::cout << "Failed to find EVEN value" << std::endl;
        }

        std::cout << "\n===============================================================\n";

        if (auto iter = std::find_if_not(ints.begin(), ints.end(), [](int v) {return ((v % 2) == 0); }); iter != ints.end()) {
            std::cout << "The first NOT EVEN value is " << *iter << std::endl;
        } else {
            std::cout << "Failed to find NOT EVEN value" << std::endl;
        }

        if (auto iter = std::find_if(ints.begin(), ints.end(), [](int v) {return ((v % 2) == 1); }); iter != ints.end()) {
            std::cout << "The first ODD is " << *iter << std::endl;
        } else {
            std::cout << "Failed to find ODD value" << std::endl;
        }
    }

    void Find_First_Of()
    {
        const std::vector<int> numbers = { 1, 2, 3, 4, 4, 3, 7, 8, 9, 10 };
        const std::vector<int> to_find = { 3, 19, 10, 2 };

        auto result = std::find_first_of(numbers.begin(), numbers.end(),
                                                to_find.begin(), to_find.end());
        std::cout << *result << std::endl;
    }

    void Find_End()
    {
        const std::vector<int> numbers = { 1,2,3,4,5,1,2,3,4,5 };

        {
            const std::vector<int> data = { 1,2,3 };
            auto result = std::find_end(numbers.begin(), numbers.end(), data.begin(), data.end());
            std::cout << "Last occurance of '{ 1,2,3 }' found at position = " << std::distance(numbers.begin(), result) << std::endl;
        }

        {
            const std::vector<int> data = { 4,5,1 };
            auto result = std::find_end(numbers.begin(), numbers.end(), data.begin(), data.end());
            std::cout << "Last occurance of '{ 4,5,1 }' found at position = " << std::distance(numbers.begin(), result) << std::endl;
        }

    }

    void Adjacent_Find() {
        const std::vector<int> numbers{ 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 41, 41, 35, 27,11, 5 };
        print_vector_ex(numbers, "Initial vector: ");

        {
            auto iter = std::adjacent_find(numbers.begin(), numbers.end());
            if (iter == numbers.end()) {
                std::cout << "no matching adjacent elements\n";
            }
            else {
                std::cout << "the first adjacent pair of equal elements at: "
                          << std::distance(numbers.begin(), iter) << ". {" << *iter << ", " << *(std::next(iter)) << "}" << std::endl;
            }
        }

        {
            auto iter = std::adjacent_find(numbers.begin(), numbers.end(), std::greater<int>());
            if (iter == numbers.end()) {
                std::cout << "The entire vector is sorted in ascending order\n";
            }
            else {
                std::cout << "The last element in the non-decreasing subsequence is at: "
                          << std::distance(numbers.begin(), iter) << ". {" << *iter << ", " << *(std::next(iter)) << "}" << std::endl;
            }
        }
    }

    void Adjacent_Find_1() {
        const std::vector<int> numbers{ 10,9,8,7,6,5,4,3,4,5,6,7,8,9,10,7,4,1 };
        print_vector_ex(numbers, "Initial vector: ");

        const auto iter = std::adjacent_find(numbers.begin(), numbers.end(), std::less<int>());
        if (iter == numbers.end()) {
            std::cout << "The entire vector is sorted in decending order\n";
            return;
        }
        else {
            auto pos = std::distance(numbers.begin(), iter);
            std::cout << "First not decresing element is numbers[" << pos << "] = " << *iter << "\n";
        }

        auto iter2 = std::adjacent_find(iter, numbers.end(), std::greater<int>());
        if (iter2 == numbers.end()) {
            std::cout << "The entire vector is sorted in ascending order\n";
        }
        else {
            auto pos1 = std::distance(numbers.begin(), iter);
            auto pos2 = std::distance(numbers.begin(), iter2);
            std::cout << "Decreasing subsequence (numbers[" << pos1 << "]=" << *iter
                      << ") - (numbers[" << pos2 << "]=" << *iter2 << ")\n";
        }
    }

    void Count() {
        const std::vector<int> ints = { 1, 2, 3, 4, 4, 3, 7, 8, 9, 10 };

        int target1 = 3, target2 = 5;
        size_t num_items1 = std::count(ints.begin(), ints.end(), target1);
        size_t num_items2 = std::count(ints.begin(), ints.end(), target2);

        std::cout << "Value: " << target1 << " count: " << num_items1 << std::endl;
        std::cout << "Value: " << target2 << " count: " << num_items2 << std::endl;
    }

    void Count_IF() {
        const std::vector<int> ints = { 1, 2, 3, 4, 4, 3, 7, 8, 9, 10 };
        size_t num_items1 = std::count_if(ints.begin(), ints.end(), [](int i) {return i % 3 == 0; });
        std::cout << "Number of entry devieded by 3: " << num_items1 << std::endl;
    }

    void ForEach() {
        std::vector<int> nums{ 3, 4, 2, 8, 15, 267 };

        auto print = [](const int& n) { std::cout << " " << n; };

        std::cout << "before:";
        std::for_each(nums.begin(), nums.end(), print);
        std::cout << std::endl;

        std::for_each(nums.begin(), nums.end(), [](int& n) { n++; });

        // calls Sum::operator() for each number
        Summ s = std::for_each(nums.begin(), nums.end(), Summ());

        std::cout << "after: ";
        std::for_each(nums.begin(), nums.end(), print);
        std::cout << std::endl;
        std::cout << "sum: " << s.sum << std::endl;
    }

    void Equal() {
        std::cout << "\n--------------------------------- Compare same strings: ----------------------------------" << std::endl;
        {
            std::string str1("qwerty"), str2("qwerty");
            std::cout << "is '" << str1 << "' == '" << str2 << "' = ";
            std::cout << std::boolalpha << std::equal(str1.begin(), str1.end(), str2.begin()) << std::endl;
        }
        std::cout << "\n--------------------------------- Compare same vectors: ----------------------------------" << std::endl;
        {
            std::vector<int> v1{ 1,2,3,4,5,6,7,8,9 }, v2{ 1,2,3,4,5,6,7,8,9 };
            std::cout << std::boolalpha << std::equal(v1.begin(), v1.end(), v2.begin()) << std::endl;
        }
        std::cout << "\n--------------------------------- Compare different vectors: ----------------------------------" << std::endl;
        {
            std::vector<int> v1{ 1,2,3,4,5,6,7,8,9 }, v2{ 1,2,3,4,5 };
            std::cout << std::boolalpha << std::equal(v1.begin(), v1.end(), v2.begin()) << std::endl;
        }
    }

    bool Is_Palindrome(const std::string& string) {
        return std::equal(string.begin(),
                          string.begin() + string.size() / 2,
                          string.rbegin());
    }

    bool Is_Reversed_Equals(const std::string& string1, const std::string& string2) {
        return std::equal(string1.begin(),
                          string1.end(),
                          string2.rbegin());
    }

    void Equal_Applications() {
        std::cout << "\n--------------------------------- Compare palindrome strings: ----------------------------------" << std::endl; {
            std::cout << std::boolalpha << Is_Palindrome("radar") << std::endl;
        }
        std::cout << "\n--------------------------------- Compare Not palindrome strings: ----------------------------------" << std::endl; {
            std::cout << std::boolalpha << Is_Palindrome("hello") << std::endl;
        }

        std::cout << "\n\n--------------------------------- Compare to reversed equals strings: ----------------------------------" << std::endl; {
            std::string str1("12345"), str2("54321");
            std::cout << str1 << " ??? " << str2 << " == ";
            std::cout << std::boolalpha << Is_Reversed_Equals(str1, str2) << std::endl;
        }
    }

    //-----------------------------------------------------------------------------------------------------------//

    template<size_t N>
    struct DivisibleByT {
        bool operator()(int n) const { return n % N == 0; }
    };

    struct DivisibleBy {
        const int d;
        DivisibleBy(int n) : d(n) {}
        bool operator()(int n) const { return n % d == 0; }
    };

    void _any_of() {
        std::vector<int> ints = { 2,4,6,8,10,12,17,14,16,18 };

        const auto devised_by_10 = [](int v)-> bool { return 0 == v % 10; };

        if (std::any_of(ints.cbegin(), ints.cend(), DivisibleBy(7))) {
            std::cout << "At least one number is divisible by 7" << std::endl;
        }
        if (std::any_of(ints.cbegin(), ints.cend(), DivisibleByT<17>())) {
            std::cout << "At least one number is divisible by 17" << std::endl;
        }
        if (std::any_of(ints.cbegin(), ints.cend(), devised_by_10)) {
            std::cout << "At least one number is divisible by 10" << std::endl;
        }
    }

    void _any_of_1() {
        std::vector<std::string> values = { "one", "two", "three", "four", "five" };
        auto result = std::any_of(values.begin(), values.end(), [](const auto& param) {
            std::cout << param << std::endl;
            return (0 == param.compare("three"));
        });
    }

    void Any_Of() {
        _any_of();
        //_any_of_1();
    }


    void All_Of()
    {
        {
            std::vector<int> numbers(10, 2);
            std::partial_sum(numbers.cbegin(), numbers.cend(), numbers.begin());
            print_vector_ex(numbers);

            if (std::all_of(numbers.cbegin(), numbers.cend(), [](int i) { return i % 2 == 0; })) {
                std::cout << "All numbers are even" << std::endl;
            }
        }
        std::cout << "---------------------------------------- Test2 ---------------------------------------" << std::endl;
        {

            auto size_equal_three = [val = 3](const std::string& str) { return str.size() == val; };
            std::vector<std::string> tokens{ "aaa", "bbb", "ccc", "ddd", "ss" };
            print_vector_ex(tokens);


            auto result = std::all_of(tokens.begin(), tokens.end(), size_equal_three);
            std::cout << "Is all strings sized == 3: " << std::boolalpha << result << std::endl;
        }

        std::cout << "---------------------------------------- Test4 ---------------------------------------" << std::endl;
        {

            int data[] = {0,0,0,0,0,0,0,0,0,0,0,0};


            auto result = std::all_of(std::begin(data), std::end(data), [](auto v) { return 0 == v; });
            std::cout << std::boolalpha << result << std::endl;
        }
    }

    void None_Of() {
        std::vector<int> ints(10, 2);
        std::partial_sum(ints.cbegin(), ints.cend(), ints.begin());
        std::cout << "Among the numbers: ";
        std::copy(ints.cbegin(), ints.cend(), std::ostream_iterator<int>(std::cout, " "));
        std::cout << std::endl;

        if (std::none_of(ints.cbegin(), ints.cend(), std::bind(std::modulus<int>(), std::placeholders::_1, 2))) {
            std::cout << "None of them are odd" << std::endl;
        }
    }

    //------------------------------------------------------------------------------------------------------//

    bool mypredicate(int i, int j) {
        return (i == j);
    }

    void Mismatch() {
        std::vector<int> numbers1 = { 10, 20, 30, 40, 50 };
        std::vector<int> numbers2 = { 10, 20, 80, 320, 1024 };

        // using default comparison:
        auto [iterator1, iterator2] = std::mismatch(numbers1.begin(), numbers1.end(), numbers2.begin(), numbers2.end());

        std::cout << "Last matching element: " << *(iterator1 - 1) << std::endl;
        std::cout << "First mismatching elements: " << *iterator1 << " and " << *iterator2 << std::endl;

        // using predicate comparison:
        auto [iterator3, iterator4] = std::mismatch(iterator1 + 1, numbers1.end(), iterator2 + 1, mypredicate);
        std::cout << "Second mismatching elements: " << *iterator3 << " and " << *iterator4 << std::endl;
    }

    void Mismatch2() {
        const std::vector<int> numbers1 = { 1, 2, 3, 4, 5, 6, 7, 8 };
        const std::vector<int> numbers2 = { 10, 20, 30, 40, 55, 60 ,70, 80 };
        auto tenTimeGreater = [](int a, int b) { return  (a * 10 == b); };


        // using default comparison:
        auto [iter1, iter2] = std::mismatch(numbers1.begin(), numbers1.end(), numbers2.begin(), tenTimeGreater);
        std::cout << "Last matching elements: {" << *(std::prev(iter1)) << ", " << *(std::prev(iter2)) << "} " << std::endl;
        std::cout << "First mismatching elements: " << *iter1 << " and " << *iter2 << std::endl;
    }

    //------------------------------------------------------------------------------------------------------//

    void Is_Permutation()
    {
        std::vector<int> v1{ 1,2,3,4,5 };
        std::vector<int> v2{ 3,5,4,1,2 };
        std::cout << "3,5,4,1,2 is a permutation of 1,2,3,4,5? " << std::boolalpha
                  << std::is_permutation(v1.begin(), v1.end(), v2.begin()) << '\n';

        std::vector<int> v3{ 3,5,4,1,1 };
        std::cout << "3,5,4,1,1 is a permutation of 1,2,3,4,5? " << std::boolalpha
                  << std::is_permutation(v1.begin(), v1.end(), v3.begin()) << '\n';
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												         MinMax :		       	    							     	    	                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Algorithms::MinMax {

    void MinMax()
    {
        std::vector<int> v{ 3, 1, 4, 1, 5, 9, 2, 6 };
        auto bounds = std::minmax(v.size(), v.size());

        std::cout << "v[" << bounds.first << "," << bounds.second << "]: ";
        for (auto i = bounds.first; i < bounds.second; ++i) {
            std::cout << v[i] << ' ';
        }
    }

    void MinMax_Element()
    {
        {
            const auto numbers = { 3, 9, 1, 4, 2, 5, 9 };
            const auto [min, max] = std::minmax_element(begin(numbers), end(numbers));
            std::cout << "min = " << *min << ", max = " << *max << std::endl;
        }

        {
            std::vector<int> v{ 3, 1, 4, 1, 5, 9, 2, 6 };
            const auto [min, max] = std::minmax_element(v.begin(), v.end());
            std::cout << "min = " << *min << ", max = " << *max << std::endl;
        }
    }

    void Max_Element()
    {
        class myclass {
        public:
            bool operator() (int i, int j) { return i < j; }
        };

        const auto myfn = [](int i, int j)-> bool { return i < j; };

        {
            std::vector<int> data = { 1, 3, 5, 124, 2 };
            auto max = *std::max_element(data.begin(), data.end());
            std::cout << max << std::endl;
        }
        std::cout << "\nTest2:" << std::endl;
        {
            int myints[] = { 3,7,2,5,6,4,9 };
            std::cout << "The largest element is " << *std::max_element(myints, myints + 7, myfn) << std::endl;
        }
        std::cout << "\nTest3:" << std::endl;
        {
            myclass pred;
            int myints[] = { 3,7,2,5,6,4,9 };
            std::cout << "The largest element is " << *std::max_element(myints, myints + 7, pred) << std::endl;
        }

        std::cout << "\nTest4 Empty set:" << std::endl;
        {
            std::vector<Integer> nums;
            auto max_el = std::max_element(nums.begin(), nums.end());
            if (nums.end() != max_el)
                std::cout << "The largest element is " << *std::max_element(nums.begin(), nums.end()) << std::endl;
            else
                std::cout << "Empty set" << std::endl;
        }
    }

    void Min_Element()
    {
        std::vector<int> data = { 1, 3, 5, 124, 2 };
        auto min = *std::min_element(data.begin(), data.end());
        std::cout << "min_element = " << min << std::endl;
    }

    void Min() {
        auto comp = [](int a, int b) {
            return (a < b);
        };

        std::cout << "\n --------------------------------------- Test0 (Default predicate): " << std::endl;
        {
            int result = std::min({ 3, 1, 4, -1, 5, 9, 2, 6 });
            std::cout << "Min of { 3, 1, 4, -1, 5, 9, 2, 6 } is " << result << std::endl;
        }

        std::cout << "\n --------------------------------------- Test1: " << std::endl;
        {
            int result = std::min({ 3, 1, 4, -1, 5, 9, 2, 6 }, comp);
            std::cout << "Min of { 3, 1, 4, -1, 5, 9, 2, 6 } is " << result << std::endl;
        }

        std::cout << "\n --------------------------------------- Test2: " << std::endl;
        {
            int result = std::min({ 3, 1, 4, -1, 5, 9, 2, 6 }, [](int a, int b) {return (a > b); });
            std::cout << "Max of { 3, 1, 4, -1, 5, 9, 2, 6 } is " << result << std::endl;
        }

        std::cout << "\n --------------------------------------- Test3: " << std::endl;
        {
            int result = std::min(1, 5);
            std::cout << "Min of {1 , 5 } is " << result << std::endl;
        }
    }

    void Max() {
        std::cout << "\n --------------------------------------- Test1: " << std::endl;
        {
            int result = std::max({ 3, 1, 4, -1, 5, 9, 2, 6 }, [](int a, int b) {return (a < b); });
            std::cout << "Max of { 3, 1, 4, -1, 5, 9, 2, 6 } is " << result << std::endl;
        }

        std::cout << "\n --------------------------------------- -Test2: " << std::endl;
        {
            int result = std::max({ 3, 1, 4, -1, 5, 9, 2, 6 }, [](int a, int b) {return (a > b); });
            std::cout << "Min of { 3, 1, 4, -1, 5, 9, 2, 6 } is " << result << std::endl;
        }

        std::cout << "\n --------------------------------------- Test3: " << std::endl;
        {
            int result = std::max(1, 5);
            std::cout << "Max of {1 , 5 } is " << result << std::endl;
        }

        std::cout << "\n --------------------------------------- Test4: " << std::endl;
        {
            std::cout << "larger of 1 and 9999: " << std::max(1, 9999) << '\n'
                      << "larger of 'a', and 'b': " << std::max('a', 'b') << '\n'
                      << "longest of \"foo\", \"bar\", and \"hello\": " <<
                      std::max({ "foo", "bar", "hello" }, [](const std::string& s1, const std::string& s2) {return s1.size() < s2.size(); }) << '\n';
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												     BinarySearch :		       	    							     	    	                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Algorithms::BinarySearch {

    void BinarySearch()
    {
        {
            std::vector<int> numbers = { 1,2,3,4,5,6,7,8,9 };
            bool result = std::binary_search(numbers.begin(), numbers.end(), 3);
            std::cout << result << std::endl;
        }

        std::cout << "\n-------------------- Test2: --------------------------\n" << std::endl;
        {
            std::vector<int> haystack{ 1, 3, 4, 5, 9 };
            std::vector<int> needles{ 1, 2, 8 };

            for (auto needle : needles) {
                std::cout << "Searching for " << needle << std::endl;
                if (std::binary_search(haystack.begin(), haystack.end(), needle)) {
                    std::cout << "Found " << needle << std::endl;
                }
                else {
                    std::cout << "no dice!" << std::endl;
                }
            }
        }
    }

    void LowerBound_UpperBound()
    {
        std::cout << "---------------------------------   Test1   --------------------------\n" << std::endl;
        {
            int start = 4, end = 8;
            std::vector<int> data = { 1, 1, 2, 3, 3, 3, 3, 4, 4, 5, 6, 8, 8, 8, 9, 10, 11, 12, 13 };
            auto lower = std::lower_bound(data.begin(), data.end(), start);
            auto upper = std::upper_bound(data.begin(), data.end(), end);

            std::cout << "Values from " << start << " - " << end << " :  ";
            std::copy(lower, upper, std::ostream_iterator<int>(std::cout, " "));
        }

        std::cout << "\n\n---------------------------------   Test2   --------------------------\n" << std::endl;

        {
            std::vector<int> data = { 1,2,3,4,5,6,7,7,7,8,9,9,11,11,12,16,20,20,20,20 };

            int start = 10, end = 202;
            auto lower = std::lower_bound(data.begin(), data.end(), start);
            auto upper = std::upper_bound(data.begin(), data.end(), end);

            std::cout << "Values from " << start << " - " << end << " :  ";
            while (++lower != upper)
                std::cout << *lower << " ";
            std::cout << std::endl;
        }
    }

    void UpperBound()
    {
        std::vector<int> data = { 1,2,3,4,5,6,7,8,9,10,11,12 };
        for (int val : {2, 4, 10}) {
            auto upper = std::upper_bound(data.begin(), data.end(), val);

            std::cout << "upper_bound(" << val << ") is ";
            if (upper != data.end())
                std::cout << *upper << " at index " << std::distance(data.begin(), upper);
            else
                std::cout << "not found";
            std::cout << std::endl;
        }
    }

    void UpperBound1()
    {
        std::vector<int> data = { 1,3,5,7,9,12 };
        for (int val : {2, 4, 10}) {
            auto upper = std::upper_bound(data.begin(), data.end(), val);

            std::cout << "upper_bound(" << val << ") is ";
            if (upper != data.end())
                std::cout << *upper << " at index " << std::distance(data.begin(), upper);
            else
                std::cout << "not found";
            std::cout << std::endl;
        }
    }


    void EqualRange() {

        std::vector<int> numbers = { 10,10,10,20,20,20,31,32,33,50,50,50,50 };

        {
            auto bounds = std::equal_range(numbers.begin(), numbers.end(), 20);

            std::cout << "bounds at indexes " << (bounds.first - numbers.begin() - 1) << " and " << (bounds.second - numbers.begin() - 1) << ":  ";
            std::for_each(bounds.first, bounds.second, [](int v) {std::cout << v << " "; });
            std::cout << std::endl;
        }


        {
            auto valid = [](int a, int b)-> bool { return a > b; };
            auto bounds = std::equal_range(numbers.begin(), numbers.end(), 20,valid);

            std::cout << "bounds at indexes " << (bounds.first - numbers.begin() - 1) << " and " << (bounds.second - numbers.begin() - 1) << ":  ";
            std::for_each(bounds.first, bounds.second, [](int v) {std::cout << v << " "; });
            std::cout << std::endl;
        }

    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//									              Set operations (on sorted ranges) :		       	             				                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Algorithms::Sorted_Set_Operations {

    void Set_Difference() {
        std::vector<int> first = { 5,10,15,20,25 }, second = { 50,40,30,20,10 };

        std::sort(first.begin(), first.end());
        std::sort(second.begin(), second.end());

        std::vector<int> result(first.size() + second.size());
        auto iter = std::set_difference(first.begin(), first.end(), second.begin(), second.end(), result.begin());

        result.resize(iter - result.begin());     // {5 15 25  0  0  0  0  0  0  0}   ===>  {5 10 15 20 25 30 40 50}
        std::cout << result << std::endl;
    }

    void Set_Intersection() {
        std::vector<int> first = { 1,2,3,4,5,6,7,8 }, second = { 5,  7,  9,10 }, result;
        std::set_intersection(first.begin(), first.end(),
                              second.begin(), second.end(), std::back_inserter(result));
        std::cout << "Result: " << result << std::endl;
    }


    void Set_Union() {
        std::vector<int> first = { 5,10,15,20,25 }, second = { 50,40,30,20,10 };

        std::sort(first.begin(), first.end());
        std::sort(second.begin(), second.end());

        std::vector<int> result(first.size() + second.size());
        auto iter = std::set_union(first.begin(), first.end(), second.begin(), second.end(), result.begin());

        result.resize(iter - result.begin());     // {5 10 15 20 25 30 40 50  0  0}   ===>  {5 10 15 20 25 30 40 50}
        print_vector_ex(result);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												       Sorting :		       	     							     	    	                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Algorithms::Sorting {

    void IsSorted() {
        std::vector<int> numbers1 = { 1,2,3,4,5,6,7,8,9 };
        std::vector<int> numbers2 = { 1,2,3,4,9,5,6,8 };

        std::cout << "Is sorted: " << std::boolalpha << std::is_sorted(numbers1.begin(), numbers1.end()) << std::endl;
        std::cout << "Is sorted: " << std::boolalpha << std::is_sorted(numbers2.begin(), numbers2.end()) << std::endl;
    }

    void IsSorted_until() {
        std::vector<int> numbers1 = { 1,2,3,4,5,6,7,8,9 };
        std::vector<int> numbers2 = { 1,2,3,4,9,5,6,8 };

        auto iter_until = is_sorted_until(numbers1.begin(), numbers1.end());
        if (numbers1.end() == iter_until)
            std::cout << "First array fully sorted" << std::endl;
        else
            std::cout << "First sorted until " << std::distance(numbers1.begin(), iter_until) << std::endl;

        iter_until = is_sorted_until(numbers2.begin(), numbers2.end());
        if (numbers2.end() == iter_until)
            std::cout << "Second array fully sorted" << std::endl;
        else
            std::cout << "Second sorted until " << std::distance(numbers2.begin(), iter_until) << std::endl;
    }

    void Sort() {
        std::array<int, 10> numbers = { 5, 7, 4, 2, 8, 6, 1, 9, 0, 3 };

        std::sort(numbers.begin(), numbers.end());
        std::copy(numbers.begin(), numbers.end(), std::ostream_iterator<int>(std::cout, " "));
        std::cout << std::endl;

        std::sort(numbers.begin(), numbers.end(), std::greater<int>());
        std::copy(numbers.begin(), numbers.end(), std::ostream_iterator<int>(std::cout, " "));
        std::cout << std::endl;
    }

    void PartialSort()
    {
        std::cout << "\n---------------------- test 1 -----------------------------\n" << std::endl;
        {
            std::array<int, 10> numbers = { 19,8,7,-12,0, 34,523, 78,-3, 4 };
            std::cout << numbers << std::endl;

            std::partial_sort(numbers.begin(), numbers.begin() + 3, numbers.end());
            std::cout << numbers << std::endl;
        }

        std::cout << "\n---------------------- test 2 -----------------------------\n" << std::endl;
        {
            std::array<int, 10> numbers = { 19,8,7,-12,0, 34,523, 78,-3, 4 };
            std::cout << numbers << std::endl;

            std::partial_sort(numbers.begin(), numbers.begin() + 3, numbers.end(), std::greater<int>());
            std::cout << numbers << std::endl;
        }

        std::cout << "\n---------------------- test 3 -----------------------------\n" << std::endl;
        {
            std::array<int, 10> numbers = { 9,8,7,6,5,4,3,2,1,0 };
            std::cout << numbers << std::endl;

            std::partial_sort(numbers.begin(), numbers.begin() + 1, numbers.end(), std::less<int>());
            std::cout << numbers << std::endl;
        }
    }

    void Nth_Element()
    {
        {
            std::vector<int> v{ 1,2,3,4,5,6,7,8,9 };
            std::cout << v << std::endl;

            std::nth_element(v.begin(), v.begin() + 1, v.end(), std::greater<>());
            std::cout << "The second largest element is " << v[1] << std::endl;

            std::cout << v << std::endl;
        }

        std::cout << "\n=================================== TEST2 ==================================\n" << std::endl;

        {
            std::vector<int> v{ 5, 6, 4, 3, 2, 6, 7, 9, 3 };
            std::cout << v << std::endl;
            std::nth_element(v.begin(), v.begin(), v.end(), std::greater<>());
            std::cout << "Max element: " << v[0] << std::endl;
            std::cout << v << std::endl;
        }

        std::cout << "\n=================================== TEST3 ==================================\n" << std::endl;

        {
            std::vector<int> numbers{1, 9, 2, 8, 3, 7, 4, 6, 5};
            nth_element(numbers.begin(), numbers.begin() + 5, numbers.end());
            auto less_than_6th_elem = [&elem = numbers[5]](int x) {
                return x < elem;
            };

            std::cout << std::boolalpha << (numbers[5] == 6) << std::endl;
        }
    }



    void Sort_Parralel()
    {
        constexpr unsigned long long COUNT = 100'000'000;
        auto seed = std::random_device{}();
        std::vector<int> data(COUNT);

        std::generate(std::execution::par_unseq, data.begin(), data.end(), std::mt19937{ seed });
        std::cout << std::boolalpha << std::is_sorted(data.begin(), data.end()) << std::endl;

        std::sort(std::execution::par_unseq, data.begin(), data.end());
        std::cout << std::boolalpha << std::is_sorted(data.begin(), data.end()) << std::endl;

    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										  Modifying sequence operations :		       	     							     	    	          //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Algorithms::ModifyingSequenceOperations {

    void Copy() {
        {
            int values[] = { 10,20,30,40,50,60,70 };
            std::vector<int> numbers(std::size(values));

            std::copy(values, values + std::size(values), numbers.begin());
            print_vector_ex(numbers, "");
        }
        {
            std::vector<int> src = { 1,2,3,4,5 }, dst(src.size());

            std::copy(src.begin(), src.end(), dst.begin());
            print_vector_ex(dst, "");
        }
    }

    void Copy_If() {
        const auto is_even = [](int v) { return 0 == v % 2; };
        std::vector<int> values{ 1,2,3,4,5,6,7,8,9,10 };

        std::vector<int> dest(std::count_if(values.begin(), values.end(), is_even));
        std::copy_if(values.begin(), values.end(), dest.begin(), is_even);
        std::cout << dest << std::endl;
    }

    void Copy_N() {
        std::string src = "1234567890", dst = "";

        std::cout << src << '\n';
        std::copy_n(src.begin(), 4, std::back_inserter(dst));
        std::cout << dst << '\n';
    }

    void Copy_Backward()
    {
        {
            std::vector<int> src = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }, dst(15);

            std::cout << dst << std::endl;
            std::copy_backward(src.begin(), src.end(), dst.end());
            std::cout << dst << std::endl;
        }

        {
            std::vector<Integer> src = { Integer(1), Integer(2), Integer(3) }, dst(src.size());
            std::copy_backward(src.begin(), src.end(), dst.end());

            std::cout << "to_vector contains: ";
            for (const Integer& i : dst)
                i.printInfo();
        }
    }

    void Move() {
        auto func = [](int timeout)-> void {
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            std::cout << "thread " << timeout << " ended\n";
        };

        std::vector<std::thread> src, dst;
        src.emplace_back(func, 1);
        src.emplace_back(func, 2);
        src.emplace_back(func, 3);

        // copy() would not compile, because std::thread is noncopyable !!!!!!!!!!!!!!!

        std::move(src.begin(), src.end(), std::back_inserter(dst));
        for (auto& t : dst)
            t.join();
    }

    void Move2() {
        std::vector<std::string> src{ "one", "two", "three" };
        std::vector<std::string> dst{ "11", "22", "33", "44", "55" };

        std::cout << "tests: " << src << "\ndst: " << dst << std::endl;
        std::move(src.begin(), src.end(), std::back_inserter(dst));
        std::cout << "tests: " << src << "\ndst: " << dst << std::endl;
    }

    void Move_Backward() {
        std::vector<std::string> src{ "one", "two", "three" };
        std::vector<std::string> dst{ "11", "22", "33", "44", "55" };

        std::cout << "tests: " << src << "\ndst: " << dst << std::endl;
        std::move_backward(src.begin(), src.end(), dst.end());
        std::cout << "tests: " << src << "\ndst: " << dst << std::endl;
    }

    void Fill() {
        std::vector<int> v{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        std::cout << v << std::endl;

        std::fill(v.begin(), v.end(), -1);
        std::cout << v << std::endl;
    }

    void Fill_N() {
        std::vector<int> v{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        std::cout << v << std::endl;

        std::fill(v.begin(), v.end(), -1);
        std::cout << v << std::endl;

        std::fill_n(v.begin(), 5, 77);
        std::cout << v << std::endl;
    }

    void Transform()
    {
        std::cout << "\n--------------------------------------- Test1 ----------------------------------" << std::endl;
        {
            std::string s("hello");

            std::cout << s << std::endl;
            std::transform(s.begin(), s.end(), s.begin(), (int(*)(int))std::toupper);
            std::cout << s << std::endl;
        }

        std::cout << "\n--------------------------------------- Test1 (v2) ----------------------------------" << std::endl;
        {
            constexpr int CHAR_REGISTER_OFFSET = static_cast<int>('A' - 'a');
            std::string s("hello");

            std::cout << s << std::endl;
            std::transform(s.begin(), s.end(), s.begin(), [](char c) { return c + CHAR_REGISTER_OFFSET; });
            std::cout << s << std::endl;
        }

        std::cout << "\n--------------------------------------- Test2 ----------------------------------" << std::endl;
        {
            std::vector<int> vect = { 10, 20, 30, 40 ,50 ,60 };
            print_vector_ex(vect, "");

            std::transform(vect.begin(), vect.end(), vect.begin(), [](auto v) { return ++v; });
            print_vector_ex(vect, "");

            std::transform(vect.begin(), vect.end(), vect.begin(), vect.begin(), std::plus<int>());
            print_vector_ex(vect, "");
        }
        std::cout << "\n--------------------------------------- Test3 ----------------------------------" << std::endl;
        {
            std::vector<int> dest(6);
            std::vector<int> vect = { 10, 20, 30, 40 ,50 ,60 };

            std::transform(vect.begin(), vect.end(), dest.begin(), [](int v) { return v + v; });

            print_vector_ex(vect, "");
            print_vector_ex(dest, "");
        }
        std::cout << "\n--------------------------------------- Test4 ----------------------------------" << std::endl;
        {
            std::string text = "a/b/c/d/e/f";

            std::cout << text << " ==> ";

            std::transform(text.begin(), text.end(), text.begin(), [](int c) { return '/' == c ? '\\' : c; });

            std::cout << text << std::endl;
        }
    }

    void Reverse()
    {
        {
            std::vector<int> numbers = { 1,2,3,4,5 };
            std::cout << numbers << std::endl;

            std::reverse(numbers.begin(), numbers.end());
            std::cout << numbers << std::endl;
        }
    }

    void Reverse_Copy()
    {
        {
            std::vector<int> v = { 1,2,3 }, destination(3);
            std::reverse_copy(std::begin(v), std::end(v), std::begin(destination));

            print_vector_ex(v, "Bafore: ");
            print_vector_ex(destination, "After: ");
        }

        {
            std::vector<Integer> from_vector = { Integer(1),Integer(2),Integer(3) }, to_vector(from_vector.size());
            std::reverse_copy(std::begin(from_vector), std::end(from_vector), std::begin(to_vector));

            print_vector_ex(from_vector, "Bafore: ");
            print_vector_ex(to_vector, "After: ");
        }
    }

    void Remove() {
        std::cout << "--------------------------------------- Test1 ----------------------------------" << std::endl;
        {
            std::vector<int> numbers = { 1,2,99,4,5,99,7,8,99,10 };

            print_vector_ex(numbers, "");
            std::cout << std::endl;

            auto iter = remove(numbers.begin(), numbers.end(), 99);

            print_range(numbers.begin(), iter, "[", "]");
            print_range(iter, numbers.end(), " [", "]");
        }
        std::cout << "\n\n--------------------------------------- Test2 + Delete tail ----------------------------------" << std::endl;
        {
            std::vector<int> numbers = { 1,2,99,4,5,99,7,8,99,10 };

            print_vector_ex(numbers, "");
            std::cout << std::endl;

            auto iter = remove(numbers.begin(), numbers.end(), 99);
            numbers.erase(iter, numbers.end());

            print_range(numbers.begin(), iter, "[", "]");
            print_range(iter, numbers.end(), " [", "]");
        }
    }

    void Remove_SharedPtr() {
        std::vector<std::shared_ptr<Integer>> numbers;
        numbers.reserve(10);

        std::shared_ptr<Integer> integer5;
        for (int i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
            if (5 == i)
                integer5 = numbers.emplace_back(std::make_shared<Integer>(i));
            else
                numbers.emplace_back(std::make_shared<Integer>(i));
        }

        std::for_each(numbers.begin(), numbers.end(), [](const auto& i) {
            std::cout << i.get() << "  ";
            i->printInfo();
        });

        auto iter = remove_if(numbers.begin(), numbers.end(), [&integer5](const auto& ptr) {
            return (ptr.get() == integer5.get());
        });
    }

    void Remove_If()  {

        auto greater = []<typename T>(T x) { return true; };

        auto result = greater(22);

        /*
        std::cout << "--------------------------------------- Test1 ----------------------------------" << std::endl;
        {
            std::vector<int> v = { 1,2,99,4,5,99,7,8,99,10 };

            std::cout << v << std::endl;

            std::vector<int>::iterator iter = remove_if(v.begin(), v.end(), greater<10>);

            std::cout << v << std::endl;
            std::for_each(v.begin(), iter, [](int el) { std::cout << el << " ";  });
            std::cout << std::endl;
        }
        */
    }

    void Remove_Copy() {
        int myints[] = { 10,20,30,30,20,10,10,20 };               // 10 20 30 30 20 10 10 20
        std::vector<int> myvector(8);

        std::remove_copy(myints, myints + 8, myvector.begin(), 20); // 10 30 30 10 10 0 0 0

        std::cout << "myvector contains:";
        for (std::vector<int>::iterator it = myvector.begin(); it != myvector.end(); ++it)
            std::cout << ' ' << *it;
        std::cout << '\n';
    }

    void Unique() {
        std::cout << "TEST1:" << std::endl;
        {
            std::vector<int> ints = { 1,2,3,1,2,3,3,4,5,4,5,6,7 };
            print_vector_ex(ints, "Vector contains (Before): ");
            std::sort(ints.begin(), ints.end()); // 1 1 2 2 3 3 3 4 4 5 5 6 7
            auto last = std::unique(ints.begin(), ints.end());
            ints.erase(last, ints.end());
            print_vector_ex(ints, "Vector contains (After) : ");
        }

        std::cout << "\nTEST2:\n" << std::endl;

        {
            std::string s = "wanna go    to      space?";
            std::cout << "Before : " << s << std::endl;

            auto end = std::unique(s.begin(), s.end(), [](unsigned char l, unsigned char r) {
                return std::isspace(l) && std::isspace(r);
            });
            std::cout << "After  : "  << std::string(s.begin(), end) << std::endl;
        }
    }

    void UniqueCopy() {
        {
            int myints[] = { 1,1,2,2,3,3,4,4 };
            std::vector<int> myvector(std::size(myints));

            auto it = std::unique_copy(myints, myints + std::size(myints), myvector.begin());
            print_vector_ex(myvector, "");
        }
        {
            int myints[] = { 1,2,3,4,1,2,3,4 };
            std::vector<int> myvector(std::size(myints));

            auto it = std::unique_copy(myints, myints + std::size(myints), myvector.begin());
            print_vector_ex(myvector, "");
        }
    }

    void Rotate() {
        std::vector<int> numbers(10);

        std::iota(numbers.begin(), numbers.end(), 0);
        std::cout << numbers << std::endl;

        std::rotate(numbers.begin(), numbers.begin() + 3, numbers.end());
        std::cout << numbers << std::endl;
    }

    void Generate() {
        auto func = []()-> int {
            static int i = 1;
            return i++;
        };

        {
            std::vector<int> vect(5);
            std::generate(vect.begin(), vect.end(), func);
            std::cout << vect << std::endl;
        }

        {
            std::vector<int> vect(5);
            std::generate(vect.begin(), vect.end(), [n = 0]() mutable { return n++; });
            std::cout << vect << std::endl;
        }
    }

    void Swap_Ranges() {
        std::vector<int> v  { 1, 2, 3, 4, 5 };
        std::list<int> l  { -1, -2, -3, -4, -5 };

        std::cout << "vector: " << v << std::endl;
        std::cout << "list: " << l << std::endl;

        std::swap_ranges(v.begin(), v.begin() + 3, l.begin());

        std::cout << "\nvector: " << v << std::endl;
        std::cout << "list: " << l << std::endl;
    }
}

namespace Algorithms::Functional {

    void EqualTo() {
        constexpr int var1 = 123,  var2 = 123;

        // Call directly:
        auto result = std::equal_to<int>()(1, 1);
        std::cout << std::boolalpha << result << std::endl;

        if constexpr (std::equal_to<int>()(var1, var2)) {
            std::cout << var1 << " == " << var2 << std::endl;
        }

        // TEST2:


        std::vector<int> foo { 10,20,30,40,50 }, bar {10,20,40,80,160 };
        auto [start, end] = std::mismatch(foo.cbegin(), foo.cend(), bar.cbegin(), std::equal_to<int>());
        std::cout << "First mismatching pair is: " << *start << " and " << *end << '\n';
    }

    void Not_Equal_To() {
        constexpr int var1 = 123, var2 = 123;

        // Call directly:
        std::cout << std::boolalpha << std::equal_to<int>()(1, 1) << std::endl;
        std::cout << std::boolalpha << std::invoke(std::equal_to<int>(), 1, 1) << std::endl;

        if constexpr (std::equal_to<int>()(var1, var2)) {
            std::cout << var1 << " == " << var2 << std::endl;
        }

        const std::vector<int> numbers{ 10,10,10,20,20 };
        auto pt = std::adjacent_find(numbers.cbegin(), numbers.cend(), std::not_equal_to<int>()) + 1;
        std::cout << "The first different element is " << *pt << '\n';
    }
}

namespace Algorithms::Numeric {

    void Iota() {
        {
            int numbers[10];
            std::iota(numbers, numbers + 10, 0);
            print_range(numbers, numbers + 10, "");
        }

        {
            int numbers[10];
            std::iota(numbers, numbers + 10, 100);
            print_range(numbers, numbers + 10, "");
        }

        {
            std::vector<int> numbers(10);
            std::iota(numbers.begin(), numbers.end(), 0);
            std::cout << numbers << std::endl;
        }

        {

            int src[]{ 1,2,3,4,5,1,2,3,4,5,1,2,3,4,5 };
            std::vector<int> numbers(std::size(src));


            std::iota(numbers.begin(), numbers.end(), *src);
            for (int i: numbers)
                std::cout << i << "  ";
            std::cout << std::endl;
        }
    }

    void Accumulate()
    {
        const auto multiply = [](int x, int y)-> int {
            return x * y;
        };

        const auto magic_function = [](std::string res, int x)->std::string {
            return res += (x > 5) ? "b" : "s";
        };

        std::vector<int> ints = { 1, 2, 3, 4 };
        int sum = std::accumulate(ints.begin(), ints.end(), 0);
        std::cout << "Accumulate {1, 2, 3, 4 }. [ Default ] = " << sum << std::endl;


        int product = std::accumulate(ints.begin(), ints.end(), 1, multiply);
        std::cout << "Accumulate {1, 2, 3, 4 }. [ Multiply ] = " << product << std::endl;

        std::string magic = std::accumulate(ints.begin(), ints.end(), std::string(), magic_function);

        std::cout << magic << std::endl;



        std::vector<int> ints2 = { 1, -2, -3, 4 };
        auto abs_Sum = std::accumulate(ints2.begin(), ints2.end(), 0, [](int a, int b) { return std::abs(a) + std::abs(b); });
        std::cout << abs_Sum << std::endl;
    }

    void Accumulate2()
    {
        using namespace std::string_literals;

        std::vector<int> numbers { 0, 2, -3, 5, -1, 6, 8, -4, 9 };
        std::vector<std::string> texts{ "hello"s, " "s, "world"s, "!!!"s };
        auto lsum = [](auto const s, auto const n) {return s + n; };

        int sum = std::accumulate(std::begin(numbers), std::end(numbers), 0, lsum);
        std::cout << "Sum : " << sum << std::endl;

        auto culmulative_text = std::accumulate(std::begin(texts), std::end(texts), ""s, lsum);
        std::cout << "Text: " << culmulative_text << std::endl;
    }

    void Partial_Sum() {
        std::vector<int> ints = { 1,2,3,4,5 };
        print_vector_ex(ints, "Original vector : ");


        std::cout << "The first 10 even numbers are: ";
        std::partial_sum(ints.begin(), ints.end(), std::ostream_iterator<int>(std::cout, " "));

        std::partial_sum(ints.begin(), ints.end(), ints.begin(), std::multiplies<int>());
        print_vector_ex(ints, "\n\nMult vector : ");

    }

    template<typename Ty>
    [[nodiscard]]
    Ty MultAndSum(const std::vector<Ty>& data1,
                  const std::vector<Ty>& data2) noexcept {
        const size_t size { data1.size() };
        Ty result { 0 };
        for (size_t i = 0; i < size; ++i)
            result += data1[i] * data2[i];
        return result;
    }

    void Inner_Product() {
        std::vector<double> a{0.2f, 1.1, 1, 3, 4};
        std::vector<double> b{5, 4.4, 2, 3.3, 1};

        double r1 = std::inner_product(a.begin(), a.end(), b.begin(), 0.0f);
        double r2 = MultAndSum(a, b);
        std::cout << "Inner product of a and b: " << r1 << '\n';
        std::cout << "MultAndSum    of a and b: " << r2 << '\n';
    }
}

namespace Algorithms::Parralel {


    void Sort_Parralel()
    {
        constexpr unsigned long long COUNT = 100'000'000;
        auto seed = std::random_device{}();
        std::vector<int> data(COUNT);

        std::generate(std::execution::par_unseq, data.begin(), data.end(), std::mt19937{ seed });
        std::cout << std::boolalpha << std::is_sorted(data.begin(), data.end()) << std::endl;

        std::sort(std::execution::par_unseq, data.begin(), data.end());
        std::cout << std::boolalpha << std::is_sorted(data.begin(), data.end()) << std::endl;

    }

    void ForEach_Parralel()
    {
        constexpr size_t COUNT = 1'000;
        auto seed = std::random_device{}();
        std::vector<int> data(COUNT);

        std::generate(std::execution::par_unseq, data.begin(), data.end(), std::mt19937{ seed });

        std::unordered_set<decltype(std::this_thread::get_id())> threads;
        std::for_each(std::execution::par_unseq, data.begin(), data.end(), [&](auto i) {
            threads.insert(std::this_thread::get_id());
            std::cout << std::this_thread::get_id() << std::endl;
        });

        std::cout << threads.size() << std::endl;
    }
}

namespace Algorithms::Partitioning {

    void Stable_Partition()
    {
        std::vector<int> v{0, 0, 3, -1, 2, 4, 5, 0, 7};
        std::stable_partition(v.begin(), v.end(), [](int n){
            return n>0;
        });

        std::cout << v << std::endl;
    }
}

void Algorithms::TestAll() {

    // NonModifying::All_Of();
    // NonModifying::Any_Of();
    // NonModifying::None_Of();
    // NonModifying::ForEach();
    // NonModifying::Count();
    // NonModifying::Count_IF();
    // NonModifying::Find();
    // NonModifying::Find_IF();
    // NonModifying::Find_IF_Not();
    // NonModifying::Find_First_Of();
    // NonModifying::Find_End();
    // NonModifying::Adjacent_Find();
    // NonModifying::Adjacent_Find_1();
    NonModifying::Equal();
    // NonModifying::Equal_Applications(); // is_palindrom
    // NonModifying::Mismatch();
    // NonModifying::Mismatch2();
    // NonModifying::Is_Permutation();



    // ModifyingSequenceOperations::Copy();
    // ModifyingSequenceOperations::Copy_If();
    // ModifyingSequenceOperations::Copy_N();
    // ModifyingSequenceOperations::Copy_Backward();
    // ModifyingSequenceOperations::Move();
    // ModifyingSequenceOperations::Move2();
    // ModifyingSequenceOperations::Move_Backward();
    // ModifyingSequenceOperations::Fill();
    // ModifyingSequenceOperations::Fill_N();
    // ModifyingSequenceOperations::Transform();
    // ModifyingSequenceOperations::Reverse();
    // ModifyingSequenceOperations::Reverse_Copy();
    // ModifyingSequenceOperations::Remove();
    // ModifyingSequenceOperations::Remove_SharedPtr();
    // ModifyingSequenceOperations::Remove_If();
    // ModifyingSequenceOperations::Remove_Copy();
    // ModifyingSequenceOperations::Rotate();
    // ModifyingSequenceOperations::Unique();
    // ModifyingSequenceOperations::UniqueCopy();
    // ModifyingSequenceOperations::Generate();
    // ModifyingSequenceOperations::Swap_Ranges();

    // MinMax::Min();
    // MinMax::Max();
    // MinMax::Min_Element();
    // MinMax::Max_Element();
    // MinMax::MinMax_Element();
    // MinMax::MinMax();


    // Sorting::IsSorted();
    // Sorting::IsSorted_until();
    // Sorting::Sort();
    // Sorting::PartialSort();
    // Sorting::Nth_Element();
    // Sorting::Sort_Parralel();


    // Partitioning::Stable_Partition();


    // BinarySearch::BinarySearch();
    // BinarySearch::UpperBound();
    // BinarySearch::UpperBound1();
    // BinarySearch::LowerBound_UpperBound();
    BinarySearch::EqualRange();
    // BinarySearch::Set_Union();


    // Sorted_Set_Operations::Set_Difference();
    // Sorted_Set_Operations::Set_Intersection();
    // Sorted_Set_Operations::Set_Union();



    // Functional::EqualTo();
    // Functional::Not_Equal_To();


    // Numeric::Iota();
    // Numeric::Accumulate();
    // Numeric::Accumulate2();
    // Numeric::Partial_Sum();
    // Numeric::Inner_Product();


    // Parralel::Sort_Parralel();
    // Parralel::ForEach_Parralel();



    // Uninitialized_Copy();

    // IsArithmetic();

    // Distance();

    // Next_Permutation();

    // Partition();
};
