/**============================================================================
Name        : Ranges.cpp
Created on  : 13.08.2020
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description :  C++ Ranges
============================================================================**/

#define _CRT_SECURE_NO_WARNINGS

#include "Ranges.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <array>
#include <vector>
#include <string_view>
#include <unordered_map>

#include <map>
#include <cstring>
#include <compare>
#include <ranges>
#include <span>
#include <bit>
#include <bitset>
#include <numeric>
#include <cstdint>
#include <iterator>
#include <initializer_list>
#include <concepts>
#include <span>
#include <list>
#include <iomanip>
#include <format>
#include <print>

namespace
{
    /*
    template <std::ranges::input_range RangeType>
    void print(RangeType&& range)
    {
        std::ranges::for_each(print, [](const auto& v) {
            std::cout << v << ' ';
        });
        std::cout << '\n';
    }
     */

    template <typename T>
    void print(T&& range_or_view)
    {
        std::ranges::for_each(range_or_view, [](const auto& v) {
            std::cout << v << ' ';
        });
        std::cout << '\n';
    }
}

namespace Ranges
{
    using namespace std::literals;

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

    template<typename T, size_t L>
    std::ostream& operator<<(std::ostream& ostr, const std::array<T, L>& arr) {
        for (const auto& i : arr)
            ostr << i << ' ';
        return ostr;
    }


    template<typename K, typename V>
    std::ostream& operator<<(std::ostream& stream, const std::map<K, V>& dict)
    {
        stream << "{ ";
        for (const auto& [k, v] : dict)
            stream << k << " : " << v << ", ";
        stream << "}";
        return stream;
    }

    template <class V>
    void mutate(V& v) {
        v += 'A' - 'a';
    }

    template <class K, class V>
    void mutate_map_values(std::multimap<K, V>& m, K k) {
        auto iter_pair = m.equal_range(k);
        for (auto& [_, v] : std::ranges::subrange(iter_pair.first, iter_pair.second)) {
            mutate(v);
        }
    }

    void SubRange()
    {
        auto print = [](std::string_view rem, auto const& mm) {
            std::cout << rem << "{ ";
            for (const auto& [k, v] : mm) std::cout << "{" << k << ",'" << v << "'} ";
            std::cout << "}\n";
        };

        std::multimap<int, char> mm{ {4,'a'}, {3,'-'}, {4,'b'}, {5,'-'}, {4,'c'} };
        print("Before: ", mm);
        mutate_map_values(mm, 4);
        print("After:  ", mm);
    }

    void Size()
    {
        std::vector<int> v{ 1, 2, 3, 4, 5 };
        std::cout << "ranges::size(v) == " << std::ranges::size(v) << '\n';

        auto il = { 7 };
        std::cout << "ranges::size(il) == " << std::ranges::size(il) << '\n';

        int array[] = { 4, 5 }; // array has a known bound
        std::cout << "ranges::size(array) == " << std::ranges::size(array) << '\n';

        std::cout << std::boolalpha << "is_signed: " << std::is_signed_v<decltype(std::ranges::size(v))> << '\n';
    }

    void End()
    {
        std::vector<int> numbers = { 3, 1, 4 };
        namespace ranges = std::ranges;
        if (std::ranges::find(numbers, 5) != std::ranges::end(numbers)) {
            std::cout << "found a 5 in vector v!\n";
        }

        int a[] = { 5, 10, 15 };
        if (std::ranges::find(a, 5) != std::ranges::end(a)) {
            std::cout << "found a 5 in array a!\n";
        }
    }

    void Data()
    {
        std::string s{ "Hello world!\n" };

        char a[20]; // storage for a C-style string
        std::strcpy(a, std::ranges::data(s));
        std::cout << a;
    }


    template <std::ranges::input_range R>
    void print_range(R&& r)
    {
        if (std::ranges::empty(r)) {
            std::cout << "\tEmpty\n";
            return;
        }

        std::cout << "\tElements:";
        for (const auto& element : r) {
            std::cout << ' ' << element;
        }
        std::cout << '\n';
    }



    void Filter_View()
    {
        std::array<int, 6> data{ 6,5,4,3,2,1 };
        auto is_six = [](int v) -> bool { return 6 == v; };
        for (int v : std::ranges::filter_view(data, is_six)) {
            std::cout << v << std::endl;
        }
    }

    void Filter_View_Vector()
    {
        const std::vector<int> vi{0,1,2,3,4,5,6,7,8,9};
        auto is_even = [](int v) -> bool { return 0 == v % 2; };

        std::cout << "\nConstruct filter 'on stack':\n";
        {
            std::ranges::filter_view evens(vi, is_even);
            for (auto v : evens)
                std::cout << v << ' ';
        }

        std::cout << "\nConstruct filter 'inplace':\n";
        {
            for (auto v : std::ranges::filter_view(vi, is_even))
                std::cout << v << ' ';
        }
    }


    void View_DropWhile()
    {
        const std::string text { "    Hello World" };
        std::cout << std::quoted(text) << '\n';

        auto conv = std::views::transform(
                std::views::drop_while(text, ::isspace),::toupper);

        std::string temp(conv.begin(), conv.end());
        std::cout << std::quoted(temp) << '\n';
    }


    void Join_View()
    {
        std::vector<std::string> numbers{ "hello", " ", "ranges", " ", "world"  };
        for (char c : std::ranges::join_view{ numbers }) {
            std::cout << c <<  " ";
        }
        std::cout << std::endl;
    }

    void Join()
    {
        std::vector<std::vector<int>> nested{{1, 2}, {3, 4, 5}, {6, 7}};
        auto joined = std::views::join(nested);

        for (int i : joined)
            std::println("{}", i);
    }


    std::string to_uppercase(std::string_view word)
    {
        std::string result(word);
        for (char& c : result)
            c = std::toupper(static_cast<unsigned char>(c));
        return result;
    }

    void Join_With()
    {
        const std::vector<std::string_view> words {"The", "C++", "ranges", "library" };

        std::ranges::transform_view words_up = words | std::views::transform(to_uppercase);
        std::ranges::join_with_view joined = std::views::join_with(words_up, std::string_view(" "));

        for (auto c : joined)
            std::cout << c;
    }

    void Concat_1()
    {
        std::vector<std::string> v1{"world", "hi"}, v2 { "abc", "xyz" };
        std::string arr[]{"one", "two", "three"};

        /*
        auto v1_rev = v1 | std::views::reverse;
        auto concat = std::views::concat(v1_rev, v2, arr);

        concat[0] = "hello"; // access and write

        for (auto& elem : concat)
            std::print("{} ", elem);
        */
    }

    void Split()
    {
        constexpr std::string_view words{"Hello^_^C++^_^20^_^!"sv};
        constexpr std::string_view delim{"^_^"sv};

        for (const auto word : std::views::split(words, delim))
        {
            // with string_view's C++23 range constructor:
            std::cout << std::string_view(word) << std::endl;
        }
        std::cout << '\n';
    }

}

namespace Ranges::Filters {

    template<typename T, size_t L>
    consteval std::array<T, L> build_array() {
        std::array<T, L> data{};
        std::iota(data.begin(), data.end(), 0);
        return data;
    }

    template <std::ranges::input_range RangeType>
    void print_range(RangeType&& rng)
    {
        std::cout << "Range: ";
        if (std::ranges::empty(rng)) {
            std::cout << "Empty.";
            return;
        }

        std::ranges::for_each(rng, [](const auto& v) { std::cout << v << ' '; });
        std::cout << '\n';
    }

    //----------------------------------------------------------------------------//


    void Filter_Numbers() {
        constexpr std::array<int, 6> numbers { 0, 1, 2, 3, 4, 5 };
        auto even = [](int i) { return 0 == i % 2; };

        for (int i : numbers | std::views::filter(even)) {
            std::cout << i << ' ';
        }
        std::cout << "\n";
    }

    void Filter_Numbers_2() {
        constexpr std::array<int, 10> numbers = build_array<int, 10>();
        auto evens = numbers | std::ranges::views::filter([](int v) -> bool { return 0 == v % 2; });
        std::ranges::for_each(evens, [](auto v) { std::cout << v << ' '; });
        std::cout << "\n";
    }

    void Filter_ForEach()
    {
        const std::vector<int> numbers{ 0,1,2,3,4,5,6,7,8,9 };
        auto is_even = [](int v) -> bool { return 0 == v % 2; };

        auto evens = numbers | std::ranges::views::filter(is_even);
        std::ranges::for_each(evens, [](auto v) { std::cout << v << ' '; });
    }

    void PrintRange()
    {
        constexpr std::array<int, 10> numbers = build_array<int, 10>();
        print_range(numbers);

        auto evens = numbers | std::ranges::views::filter([](int v) -> bool { return 0 == v % 2; });
        print_range(evens);
    }
}


namespace Ranges::Take
{
    /*
    template <std::ranges::input_range RangeType>
    std::ostream& operator<<(std::ostream& stream, const RangeType& range) {
        std::ranges::for_each(range, [](const auto& v) {
            std::cout << v << ' ';
        });
        return stream;
    }
    */

    void Take_View()
    {
        std::vector<int> numbers{ 0,1,2,3,4,5 };

        {
            std::ranges::take_view view{ numbers, 3 };
            std::ranges::for_each(view, [](auto v) { std::cout << v << ' '; });
            std::cout << "\n\n";
        }

        {
            auto view = std::ranges::take_view{ numbers, 3 };
            std::ranges::for_each(view, [](auto v) { std::cout << v << ' '; });
            std::cout << "\n\n";
        }

        {
            std::ranges::take_view view{ numbers, 35 };
            std::ranges::for_each(view, [](auto v) { std::cout << v << ' '; });
            std::cout << "\n\n";
        }


        {
            std::ranges::take_view view{ numbers, 35 };

            view[2] = 133;

            std::ranges::for_each(view, [](auto v) { std::cout << v << ' '; });
            std::cout << std::endl;
            std::cout << numbers << std::endl;
        }
    }

    void Take_Test_0()
    {
        const std::vector<int> numbers {1, 2, 3, 4, 5, 6, 7, 8, 9};
        const std::ranges::take_view<std::ranges::ref_view<const std::vector<int>>> first_5 = std::views::take(numbers, 5);

        print_range(first_5);
        print_range(numbers | std::views::take(5));
    }

    void Take_Test()
    {
        const std::array<std::string, 22> contacts {
            "Ariana", "Avery", "Bruce", "Brian", "Caroline", "Carl",
            "Daniel", "Donald", "Diana", "Eric", "Emma", "Florence", "George", "Harry", "Isabella", "James", "Jackson"
            "Jennifer", "Logan", "Linda", "Lucy", "Michael"
        };


        auto name_filter = [](const auto& name) { return 5 == name.size(); };
        auto add_symbol = [](const auto& name) { return std::string(name).append("!"); };

        auto res1 = contacts |
            std::views::filter(name_filter) |
            std::ranges::views::transform(add_symbol);

        print_range(res1);

        auto res_limit_5 = contacts | std::views::filter(name_filter) |
                                                  std::ranges::views::transform(add_symbol) |
                                                  std::ranges::views::take(5);

        std::ranges::for_each(res_limit_5, [](const auto& e) { std::cout << e << " "; });
        std::cout << '\n';
    }
}

namespace Ranges::Transform
{

    template <std::ranges::input_range RangeType>
    void print_range(RangeType&& rng,
                     const std::string& text = std::string(""))
    {
        if (std::ranges::empty(rng)) {
            std::cout << "Empty.";
            return;
        }

        std::ranges::for_each(rng, [](const auto& v) { std::cout << v << ' '; });
        std::cout << '\n';
    }

    void Modify_ToUpper() {
        std::string s("hello");
        std::cout << s << std::endl;

        std::ranges::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) -> unsigned char { return std::toupper(c); }
        );

        std::cout << s << std::endl;

        auto result = s | std::ranges::views::transform([](unsigned char c) -> unsigned char { 
            return std::tolower(c); }
        );

        std::cout << s << std::endl;
    }

    void Transform_View() {
        std::string text("hello");

        std::cout << text << std::endl;

        auto view = std::ranges::transform_view( text,
            [](unsigned char c) -> unsigned char { return std::toupper(c); }
        );
        std::ranges::for_each(view, [](const auto& v) { std::cout << v; });


        std::cout << "\n" << text << std::endl;
    }

    void Transform_Filter() {
        constexpr std::array<int, 6> numbers{ 6,5,4,3,2,1 };
        auto x10 = [](int i) { return i * 10; };

        print_range(numbers, "Original");

        auto result1 = numbers | std::views::transform(x10);
        print_range(result1);

        auto result2 = numbers | 
                       std::views::filter([](int i) { return 0 == i % 2; }) |
                       std::ranges::views::transform(x10); 
        print_range(result2);

        print_range(numbers, "Original");
    }


    void Transform_Filter_2() {
        const std::array<std::string, 22> 
        // const std::vector<std::string>
            contacts {"Ariana", "Avery", "Bruce", "Brian", "Caroline", "Carl",
             "Daniel", "Donald", "Diana", "Eric", "Emma", "Florence", "George", "Harry", "Isabella", "James", "Jackson"
             "Jennifer", "Logan", "Linda", "Lucy", "Michael"};

        
        auto name_filter = [](const auto& name) { return 4 == name.size(); };
        auto updater1 = [](const auto& name) { return std::string("[").append(name).append("]"); };
        auto updater2 = [](const auto& name) { return std::string("{").append(name).append("}"); };


        auto res1 = std::views::transform(std::views::filter(contacts, name_filter), updater1);
        print_range(res1);

        auto res2 = contacts | 
                    std::views::filter(name_filter) |
                    std::ranges::views::transform(updater2);
        print_range(res2);
    }

    void Test() {
        const std::array<std::string, 22>
            // const std::vector<std::string>
            contacts{ "Ariana", "Avery", "Bruce", "Brian", "Caroline", "Carl",
             "Daniel", "Donald", "Diana", "Eric", "Emma", "Florence", "George", "Harry", "Isabella", "James", "Jackson"
             "Jennifer", "Logan", "Linda", "Lucy", "Michael" };



        auto name_filter = [](const auto& name) { return 5 == name.size(); };
        auto add_symbol = [](const auto& name) { return std::string(name).append("!"); };

        auto res1 = contacts | 
                    std::views::filter(name_filter) | 
                    std::ranges::views::transform(add_symbol);

        print_range(res1);

        auto res_limit_5 = contacts |
                           std::views::filter(name_filter) |
                           std::ranges::views::transform(add_symbol) |
                           std::ranges::views::take(5);

        std::ranges::for_each(res_limit_5, [](const auto& e) { std::cout << e << " "; });
        std::cout << '\n';
    }
}
	
namespace Ranges::Algorithms
{
    struct User {
        int64_t id;
        std::string name;
    };

    auto is_even = [](int v) -> bool { return 0 == v % 2; };
    auto print = [](int v) { std::cout << v << ' '; };

    void For_Each() {
        const std::vector<int> numbers{ 0,1,2,3,4,5,6,7,8,9 };
        auto print = [](int v) { std::cout << v << ' '; };
        std::ranges::for_each(numbers, print);
    }

    void Sort()
    {
        std::array<int, 5> numbers{5,4,3,2,1};

        std::for_each(numbers.begin(), numbers.end(), print);
        std::cout << std::endl;

        std::ranges::sort(numbers);

        std::for_each(numbers.begin(), numbers.end(), print);
        std::cout << std::endl;
    }

    void Sort_ByID()
    {
        std::vector<User> users{
                {37,"Eliana Green"},
                {23, "Logan Sterling"},
                {1, "Isla Bennett"},
                {7, "Marcel Jones"}
        };

        std::ranges::sort(users, {}, &User::id);

        for (const auto& u : users)
            std::cout << u.id << ": " << u.name << "\n";
    }


    void Find_byName()
    {
        std::vector<User> users{
            {37,"Eliana Green"}, {23, "Logan Sterling"},
            {1, "Isla Bennett"}, {7, "Marcel Jones"}
        };

        auto it = std::ranges::find(users, std::string {"Eliana Green"}, &User::name);
        // it->id == 37, it->name == "Eliana Green"
        std::cout << it->id << ": " << it->name << "\n";
    }


    void Unique() // ERROR!
    {
        std::vector<int> numbers{ 5,4,3,5, 3,2,1,2,2 };

        auto [first, last] = std::ranges::unique(numbers.begin(), numbers.end());
        numbers.erase(first, last);

        std::for_each(numbers.begin(), numbers.end(), print);
        std::cout << std::endl;
    }

    void Find_IF()
    {
        constexpr std::array<int, 6> data{ 6,5,4,3,2,1 };
        constexpr auto is_six = [](int v) -> bool { return 6 == v; };

        /*
        {
            constexpr auto result = std::ranges::find_if(data, is_six);
            if constexpr (result != std::ranges::end(data)) {
                std::cout << "Result = " << *result << std::endl;
            }
            else {
                std::cout << "Not found" << std::endl;
            }
        }

        {
            constexpr auto result = std::ranges::find_if(data, [](int v) -> bool { return 11 == v; });
            if constexpr  (result != std::ranges::end(data)) {
                std::cout << "Result = " << *result << std::endl;
            }
            else {
                std::cout << "Not found" << std::endl;
            }
        }
        */
    }

    void Sort_BackWards()
    {
        std::array<int, 5> array{ 5,4,3,2,1 };
        auto reverse_compare = [](int a, int b) {return a > b; };

        std::ranges::for_each(array, print);
        std::cout << std::endl;

        std::ranges::sort(array);

        std::ranges::for_each(array, print);
        std::cout << std::endl;

        std::ranges::sort(array, reverse_compare);

        std::ranges::for_each(array, print);
        std::cout << std::endl;
    }

    void Reverse()
    {
        const std::vector<int> numbers{ 0,1,2,3,4,5,6,7,8,9 };
        std::ranges::for_each(std::ranges::reverse_view(numbers), [](auto v) {
            std::cout << v << ' ';
        });
        std::cout << std::endl;
    }

    void Reverse_Views()
    {
        const std::vector<int> numbers{ 0,1,2,3,4,5,6,7,8,9 };

        auto reversed = numbers | std::views::reverse;
        for (int i : reversed)
            std::cout << i << " ";
        std::cout << std::endl;

        // same as:
        for (int i : numbers | std::views::reverse)
            std::cout << i << " ";
        std::cout << std::endl;

        // same as:
        std::ranges::reverse_view rv {numbers};
        for (int i : rv)
            std::cout << i << " ";
        std::cout << std::endl;
    }

    void Reverse_Span_Part() {
        auto print = [] (std::span<int> span) {
            std::ranges::for_each(std::ranges::reverse_view{ span }, [](const auto& v) {
                std::cout << v << ' ';
            });
            std::cout << std::endl;
        };

        std::vector<int> numbers{ 0,1,2,3,4,5,6,7,8,9 };
        for (int i = 0; auto _ : numbers) {
            print(std::span<int>(numbers).first(++i));
        }
        for (int i = numbers.size(); auto _ : numbers) {
            print(std::span<int>(numbers).first(--i));
        }
    }
}

namespace Ranges::Iota
{
    void CreateView_DropAndTake()
    {
        auto from_3_to_10 = std::views::iota(1)
                            | std::views::drop(3) | std::views::take(7);

        print(from_3_to_10);
    }


    void CreateView_WithTransform()
    {
        auto result = std::views::iota(1)
                      | std::views::filter([](int element) { return 0 == element % 2; })
                      | std::views::drop(3) | std::views::take(7);

        print(result);
    }
}


namespace Ranges
{
    void Experiments()
    {
        auto from_3_to_10 = std::views::iota(1)
                | std::views::drop(3) | std::views::take(7);

        print(from_3_to_10);
    }
}


namespace Ranges::Views
{
    void Repeat()
    {
        // bounded overload
        for (auto s: std::views::repeat("C++"sv, 3))
            std::cout << s << ' ';
        std::cout << '\n';

        // unbounded overload
        for (auto s : std::views::repeat("Hello"sv) | std::views::take(3))
            std::cout << s << ' ';
        std::cout << "...\n";
    }

    void Zip()
    {
        std::vector<int> nums { 1,2,3,4,5 };
        std::vector<std::string> nums1 { "one", "two", "three", "four", "five" };
        std::vector<char> nums2 {'A', 'B', 'C', 'D', 'E', 'F'};


        // Iterate over the elements of the zip view
        for (auto [a, b, c] : std::views::zip(nums, nums1, nums2)) {
            std::cout << std::format("[{}, {}, {}]\n", a, b, c);
        }


        for (std::tuple<int&, std::string&, char&> elem : std::views::zip(nums, nums1, nums2))
        {
            std::cout << std::get<0>(elem) << ' '
                      << std::get<1>(elem) << ' '
                      << std::get<2>(elem) << '\n';

            std::get<char&>(elem) += ('a' - 'A'); // modifies the element of z
        }
    }

    void Enumerate()
    {
        std::vector<int> range1 = {1, 2, 3};
        const std::ranges::enumerate_view enumerated = std::views::enumerate(range1);
        for (const auto& [index, value] : enumerated)
            std::cout << "Index: " << index << ", Value: " << value << std::endl;
    }


    /**
    * The C++20 std::views::elements takes a range of tuple-like objects and produces
    * a view over the n-th element from each tuple.
    * The concept of tuple-like was formalized in C++23 and includes std::array, std::complex (C++26),
    * std::pair, std::tuple and std::ranges::subrange.
    **/
    void Elements()
    {
        std::vector<std::pair<int,double>> data{{1,2.7}, {3, 4.2}, {-1, 3.3}};

        for (const auto v : data | std::views::elements<0>) {
            std::cout << v << ' ';
        } // 1, 3, -1

        std::cout << std::endl;

        for (const auto v : data | std::views::elements<1>) {
            std::cout << v << ' ';
        } // 2.7, 4.2, 3.3
        std::cout << std::endl;

        std::array<std::array<int,3>,3> grid{1,2,3,4,5,6,7,8,9};
        for (const auto v : grid | std::views::elements<2>) {
            std::cout << v << ' ';
        } // 3, 6, 9

        std::cout << std::endl;}
}

namespace Ranges::Ranges_To
{
    void Get_Even_Numbers()
    {
        const std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

        const auto even_numbers = numbers
                        | std::views::filter([](int n) { return n % 2 == 0; })
                        | std::ranges::to<std::vector>();

        std::cout << even_numbers << std::endl; // Output: 2 4 6 8 10
    }

    void Get_Even_Numbers_Mapping()
    {
        const std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        const std::unordered_map<int, std::string> number_to_text = {
            {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}, {5, "five"},
            {6, "six"}, {7, "seven"}, {8, "eight"}, {9, "nine"}, {10, "ten"}
        };

        const auto even_numbers = numbers
                        | std::views::filter([](const int n) { return n % 2 == 0; })
                        | std::ranges::to<std::vector>();

        auto text_numbers_map = even_numbers | std::views::transform([&number_to_text](int n) {
                    return std::pair{n, number_to_text.contains(n) ? number_to_text.at(n) : "unknown" };
                  }) | std::ranges::to<std::map>();

        std::cout << text_numbers_map << std::endl;
        // OUTPUT: { 2 : two, 4 : four, 6 : six, 8 : eight, 10 : ten, }
    }
}

void Ranges::TestAll()
{
    // For_Each();

    // Size();
    // End();
    // Data();

    // Split();

    // Filter_View();
    // Filter_View_Vector();

    // View_DropWhile();
    // Join();
    // Join_View();
    Join_With();

    // Concat_1();



    // Views::Zip();
    // Views::Repeat();
    // Views::Elements();
    // Views::Enumerate();

    // Algorithms::For_Each();
    // Algorithms::Find_IF();
    // Algorithms::Find_byName();
    // Algorithms::Sort();
    // Algorithms::Sort_ByID();
    // Algorithms::Unique();
    // Algorithms::Sort_BackWards();

    // Algorithms::Reverse();
    // Algorithms::Reverse_Views();
    // Algorithms::Reverse_Span_Part();

    // Filters::Filter_Numbers();
    // Filters::Filter_Numbers_2();
    // Filters::Filter_ForEach();
    // Filters::PrintRange();

    // Transform::Transform_Filter();
    // Transform::Transform_Filter_2();
    // Transform::Modify_ToUpper();
    // Transform::Transform_View();
    // Transform::Test();

    // Iota::CreateView_DropAndTake();
    // Iota::CreateView_WithTransform();

    // Take::Take_View();
    // Take::Take_Test_0();
    // Take::Take_Test();

    // Ranges_To::Get_Even_Numbers();
    // Ranges_To::Get_Even_Numbers_Mapping();

    // Experiments();

}
