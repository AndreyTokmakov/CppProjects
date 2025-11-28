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
#include <fstream>
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
#include <filesystem>

#include "../Helpers/Wrapper.h"

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

    std::string to_uppercase(std::string_view word)
    {
        std::string result(word);
        for (char& c : result)
            c = std::toupper(static_cast<unsigned char>(c));
        return result;
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
}

namespace Ranges::Filters
{

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
}

namespace Ranges::Filters
{
    void Filter_Numbers()
    {
        constexpr std::array<int, 6> numbers { 0, 1, 2, 3, 4, 5 };
        auto even = [](int i) { return 0 == i % 2; };

        for (int i : numbers | std::views::filter(even)) {
            std::cout << i << ' ';
        }
        std::cout << "\n";
    }

    void Filter_Numbers_2()
    {
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

namespace Ranges::Filters
{
    template<typename T, typename Pred>
    void process_range(std::ranges::filter_view<T, Pred> range)
    {
        std::cout << std::string(80, '-') << '\n';
        std::ranges::for_each(range, [](const auto& v) {
            std::cout << v << std::endl;
        });
        std::cout << std::string(80, '-') << '\n';
    }

    void Pass_Range_to_the_Function__Predicate()
    {
        using Helpers::Integer;

        const std::vector<Integer> numbers = [] {
            std::vector<Integer> tmp;
            tmp.reserve(10);
            for (int i = 0; i < 10; ++i)
                tmp.emplace_back(i);
            return tmp;
        }();

        auto is_even = [](const Integer& integer) -> bool {
            std::cout << "calling is_even(" << integer.value << ")\n";
            return false == (integer.value & 1);
        };

        std::ranges::filter_view evens = numbers | std::ranges::views::filter(is_even);
        process_range(evens);
    }
}


namespace Ranges::Split
{
    void Split_String_Simple()
    {
        {
            constexpr std::string_view words {"Hello^_^C++^_^20^_^!"sv};
            for (const std::ranges::subrange<const char*> part : std::views::split(words, "^_^"sv)) {
                std::cout << std::string_view(part) << ' ';
            }
            std::cout << '\n';
        }

        {
            constexpr  std::string_view  text = "C++breakisbreakpowerfulbreakandbreakelegant"sv;

            for (const std::ranges::subrange<const char*> part : std::views::split(text, "break"sv))
                std::print("'{} ' ", std::string_view(part));
        }
    }

    void Split_Non_String()
    {
        using Point = std::pair<int, int>;
        std::vector<Point> path = {
            {0, 0}, {1, 1}, {-1, -1},
            {2, 2}, {3, 3}, {-1, -1},
            {4, 4}, {5, 5}
        };

        for (auto segment : std::views::split(path, Point{-1, -1}))
            std::print("Segment: {}\n", segment);

        // Segment: [(0, 0), (1, 1)]
        // Segment: [(2, 2), (3, 3)]
        // Segment: [(4, 4), (5, 5)]
    }

    void Chunk_Fixed_Size_Batches()
    {
        const std::vector<int> data {1, 2, 3, 4, 5, 6, 7, 8};
        for (auto chunk : data | std::views::chunk(3))
            std::print("{}\n", chunk);

        // views::chunk splits the sequence into groups of three elements.
        // If the number of elements isn’t divisible by 3, the last chunk will contain fewer elements.
        // [1, 2, 3]
        // [4, 5, 6]
        // [7, 8]
    }

    void Chunk_Processing_Network_Packets()
    {
        std::istringstream stream {"AB CD EF 12 34 56 78 95 FF"};

        auto bytes = std::ranges::istream_view<std::string>(stream);
        for (auto packet : bytes | std::views::chunk(4))
            std::print("Packet: {}\n", packet);

        // This example simulates processing a byte stream in fixed 2-byte packets.
        // Packet: ["AB", "CD", "EF", "12"]
        // Packet: ["34", "56", "78", "95"]
        // Packet: ["FF"]
    }

    void Chunk_By_Dynamic_Grouping()
    {
        const std::vector<int> values {1, 3, 5, 2, 4, 6, 7, 9, 8};

        for (auto group : values | std::views::chunk_by([](const int a, const int b) {
            return (a % 2) == (b % 2); // Same parity
        })) {
            std::print("size {}, {}\n", group.size(), group);
        }

        // size 3, [1, 3, 5]
        // size 3, [2, 4, 6]
        // size 2, [7, 9]
        // size 1, [8]
    }

    void Chunk_By_Extracting_Sentences_from_Text()
    {
        constexpr std::string_view text = "C++ is powerful. Ranges are elegant. This is fun!"sv;

        for (auto sentence : text | std::views::chunk_by([](char a, char b) {
             // Group until a dot is found; start a new group after '.'
             return a != '.' && b != '.';
         })) {
            // Remove leading spaces if any, and skip dots-only groups
            auto view = std::string_view(&*sentence.begin(), std::ranges::distance(sentence));
            view.remove_prefix(std::min(view.find_first_not_of(' '), view.size()));

            if (!view.empty() && view != ".")
                std::print("Sentence: [{}]\n", view);
         }

        // Sentence: [C++ is powerful]
        // Sentence: [Ranges are elegant]
        // Sentence: [This is fun!]
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

        [[maybe_unused]]
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
        [[maybe_unused]]
        constexpr std::array<int, 6> data{ 6,5,4,3,2,1 };

        [[maybe_unused]]
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

namespace Ranges::Containers_From_Ranges
{
    void CreateVectorFromRange()
    {
        const std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
         std::ranges::filter_view even_numbers_view = numbers
                | std::views::filter([](int n) { return (n & 0x01) == 0; });

        const std::vector<int> evenNumbers(std::from_range, even_numbers_view);
        for (const int v: evenNumbers) {
            std::cout << v << std::endl;
        }
    }

    void CreateVectorFromRange_Inplace()
    {
        const std::vector<int> data {1,2,3,4,5,6,7,8,9};
        const std::vector<int> result = std::vector<int>(std::from_range,data
                | std::views::filter([](int n) -> bool { return (n & 0x01) == 0; })
                | std::views::transform([](int n) -> int { return n * n; })
        );

        for (int x : result) {
            std::cout << x << ' ';   // 4 16 36 64
        }
    }
}

namespace Ranges::Join
{
    struct Person
    {
        std::string name;
        uint16_t age { 0 };
    };

    void Join_View()
    {
        const auto bits = {"https:"sv, "//"sv, "cppreference"sv, "."sv, "com"sv};
        for (char const c : bits | std::views::join)
            std::print("{}", c);
        std::cout << '\n';

        const std::vector<std::vector<int>> v{{1, 2}, {3, 4, 5}, {6}, {7, 8, 9}};
        auto jv = std::ranges::join_view(v);
        for (int const e : jv)
            std::print("{} ", e);
        std::cout << '\n';

        // https://cppreference.com
        // 1 2 3 4 5 6 7 8 9
    }

    void Join()
    {
        std::vector<std::vector<int>> nested{{1, 2}, {3, 4, 5}, {6, 7}};
        auto joined = std::views::join(nested);

        for (int i : joined)
            std::print("{} ", i);

        // 1 2 3 4 5 6 7
    }

    void Join_With_1()
    {
        const std::vector<std::string_view> parts {"This"sv, "is"sv, "a"sv, "test."sv};
        const auto joined = parts | std::views::join_with(' ');

        for (const auto c : joined)
            std::cout << c;
        std::cout << '\n';

        // This is a test.
    }

    void Join_With_2()
    {
        const std::vector<std::string_view> words {"The", "C++", "ranges", "library" };

        std::ranges::transform_view words_up = words | std::views::transform(to_uppercase);
        std::ranges::join_with_view joined = std::views::join_with(words_up, std::string_view(" "));

        for (const auto c : joined)
            std::print("{}", c);
        // THE C++ RANGES LIBRARY
    }

    void Join_With_to_String()
    {
        const std::vector<std::string_view> words {"One", "Two", "Three" };
        constexpr std::string_view delimiter { " " };

        const std::string result = words | std::views::join_with(delimiter) | std::ranges::to<std::string >();
        std::println("{}", result);

        /// Output:  One Two Three
    }

    void Join_Get_ClassParameters()
    {
        std::vector<Person> persons {
            Person { "John Snow", 20 },
            Person { "Aria Start", 15 }
        };

        const auto namesView = persons | std::views::transform([](const Person& p) { return p.name; })
             | std::views::join;

        /*
        std::ranges::for_each(namesView, [](const auto& entry) {
            //std::print("{} ", name);
        });*/
    }
}

namespace Ranges::Join
{

    struct File
    {
        std::string name;
    };

    struct Project
    {
        std::vector<File> files;
    };

    struct Diagnostics
    {
        std::string info;

        explicit Diagnostics(const File& file, const std::string& info):
            info { std::format("File: {}, Info: {}", file.name, info) } {

        }
    };

    void print(const Diagnostics& diagnostics)
    {
        std::cout << diagnostics.info << std::endl;
    }

    std::vector<File> getProjectFiles(const Project& project)
    {
        return project.files;
    }

    std::vector<Diagnostics> inspect(const File& file)
    {
        std::vector<Diagnostics> diagnostics;
        diagnostics.emplace_back(file, "Info#1");
        diagnostics.emplace_back(file, "Info#2");

        return diagnostics;
    }

    std::vector<Project> getProjects()
    {
        std::vector<Project> projects;

        projects.emplace_back().files.emplace_back("P1-File_1");
        projects.back().files.emplace_back("P1-File_2");
        projects.back().files.emplace_back("P1-File_3");

        projects.emplace_back().files.emplace_back("P2-File_1");
        projects.back().files.emplace_back("P2-File_2");
        projects.back().files.emplace_back("P2-File_3");

        projects.emplace_back().files.emplace_back("P3-File_1");
        projects.back().files.emplace_back("P3-File_2");
        projects.back().files.emplace_back("P3-File_3");

        return projects;
    }

    void oldStyle()
    {
        const std::vector<Project> projects = getProjects();
        for (const auto& project: projects)
        {
            const std::vector<File> files = getProjectFiles(project);
            for (const File& file: files)
            {
                const std::vector<Diagnostics> diagnostics = inspect(file);
                for (const Diagnostics& d: diagnostics)
                {
                    print(d);
                }
            }
        }
    }

    /** https://youtu.be/YUHbPDNtdiQ?t=1562 **/
    void rangeJoinStyle()
    {
       auto diagnostics = getProjects() |
               std::views::transform(getProjectFiles) | std::views::join |
               std::views::transform(inspect) | std::views::join ;
       std::ranges::for_each(diagnostics, print);
    }
}

namespace Ranges::Map_to_Vector_of_Values
{
    template<class KeyType, class ValueType>
    std::vector<ValueType> getValues(const std::map<KeyType, ValueType>& map)
    {
        return map | std::views::values | std::ranges::to<std::vector>();
    }

    template<class KeyType, class ValueType>
    std::vector<ValueType> getValues2(const std::map<KeyType, ValueType>& map)
    {
        return  std::views::values(map) | std::ranges::to<std::vector>();
    }

    void collectValues()
    {
        const auto map = std::map<int, std::string> {
            { 24, "Hello" },
            { 42, "World" }
        };

        {
            for (auto&& value : getValues(map))
                std::println("{}", value);
        }

        {
            for (auto&& value : getValues2(map))
                std::println("{}", value);
        }
    }
}


namespace Ranges::Slide
{
    auto print_subrange = [](const std::ranges::viewable_range auto& r)
    {
        std::cout << "[ ";
        for (auto& v : r)
            std::cout << v << ' ';
        std::cout << "] ";
    };

    void Simple_Example()
    {
        const std::vector<int> numbers { 1, 2, 3, 4, 5 };

        std::cout << "All sliding windows of width:\n";
        for (const unsigned width : std::views::iota(1U, 1U + numbers.size()))
        {
            const std::ranges::slide_view window_views = numbers | std::views::slide(width);
            std::cout << "W = " << width << ": ";
            for (const auto& window : window_views) {
                print_subrange(window);
            }
            std::cout << '\n';
        }

        // All sliding windows of width:
        // W = 1: [ 1 ] [ 2 ] [ 3 ] [ 4 ] [ 5 ]
        // W = 2: [ 1 2 ] [ 2 3 ] [ 3 4 ] [ 4 5 ]
        // W = 3: [ 1 2 3 ] [ 2 3 4 ] [ 3 4 5 ]
        // W = 4: [ 1 2 3 4 ] [ 2 3 4 5 ]
        // W = 5: [ 1 2 3 4 5 ]
    }
}

namespace Ranges::Ranges_Sort
{
    struct Time
    {
        int hours { 0 };
        int minutes { 0 };
        int seconds { 0 };

        /*
        bool operator<(const Time &other) const {
            return std::tie(hours, minutes, seconds) < std::tie(other.hours, other.minutes, other.seconds);
        }*/

        // Spaceship operator (генерирует все 6 операторов сравнения)
        auto operator<=>(const Time& other) const = default;
    };

    std::ostream& operator<<(std::ostream& stream, const Time& time)
    {
        stream << std::format("{}:{}:{}", time.hours, time.minutes, time.seconds);
        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const std::vector<Time>& times)
    {
        for (const auto& time: times) {
            stream << time << std::endl;
        }
        return stream;
    }

    void Sort_Custom_Type()
    {
        std::vector<Time> times = {
            {14, 30, 15},
            {9, 45, 0},
            {14, 30, 0},
            {23, 59, 59},
            {0, 0, 0}
        };

        // По умолчанию в сортировке рэнджей используется компаратор std::ranges::less, а в обычной сортировке - std::less.
        // И в их разнице и зарыта собака: std::ranges::less требует определения всех шести операторов
        // сравнения или одного spaceship'а и все заработает:

        std::ranges::sort(times);

        std::cout << times << std::endl;
    }
}

namespace Ranges::Max_Element
{
    struct Payment
    {
        double amount { 0.0 };
        std::string category;

        auto operator<=>(const Payment& other) const = default;
    };

    std::ostream& operator<<(std::ostream& stream, const Payment& payment)
    {
        return stream << "Payment{amount: " << payment.amount << ", category: " << payment.category <<" }";
    }

    void Find_Max_Element_by_Class_Field()
    {
        std::vector<Payment> payments {
	            {100.0, "food"},
                {200.0, "transport"},
                {150.0, "food"},
                {300.0, "entertainment"},
                {50.0, "transport"},
                {250.0, "food"},
                {120.0, "food"}
        };

        Payment& maxPayment = *std::ranges::max_element(payments, {}, &Payment::amount);
        std::cout << maxPayment << std::endl;

        /// Output:
        //      Payment{amount: 300, category: entertainment }
    }
}


namespace Ranges::Keys_Values_of_Map
{
    struct UserProperties {};

    std::vector<std::string> getUsernamesFromMap(const std::map<std::string, UserProperties>& usersMap)
    {
        return usersMap | std::views::keys | std::ranges::to<std::vector>();
    }

    std::vector<int> getValuesFromMap(const std::map<std::string, int>& usersMap)
    {
        return usersMap | std::views::values | std::ranges::to<std::vector>();
    }

    void getAllKeysFromMap()
    {
        std::map<std::string, UserProperties> usersMap = {{"John", UserProperties{}}, {"Peter", UserProperties{}}};
        std::vector<std::string> usernames = getUsernamesFromMap(usersMap);
        std::ranges::for_each(usernames, [](const auto& username) {
            std::println("{}", username);
        });

        // John
        // Peter
    }

    void getAllValuesFromMap()
    {
        std::map<std::string, int> data = {{"John", 1}, {"Peter", 2}};
        std::vector<int> usernames = getValuesFromMap(data);
        std::ranges::for_each(usernames, [](const auto& username) {
            std::println("{}", username);
        });

        // 1
        // 2
    }
}

namespace Ranges::Files
{
    std::filesystem::path getDataDir()
    {
        return std::filesystem::current_path() / "../../resources";
    }

    std::vector<std::string> loadUrls(std::istream& stream)
    {
        return std::views::istream<std::string>(stream) | std::ranges::to<std::vector<std::string>>();
    }

    void parsing_Input_Files()
    {
        const std::filesystem::path urlListFile = getDataDir() / "urls.txt";
        if (std::fstream file (urlListFile.string()); file.is_open()) {
            const std::vector<std::string> urls = loadUrls(file);
            std::ranges::for_each(urls, [](const auto& url) { std::println("{}", url); });
        }
    }
}

void Ranges::TestAll()
{
    // For_Each();

    // Size();
    // End();
    // Data();

    // Filter_View();
    // Filter_View_Vector();

    // View_DropWhile();

    // Map_to_Vector_of_Values::collectValues();

    // Join::oldStyle();
    // Join::Join();
    // Join::Join_View();
    // Join::Join_With_1();
    // Join::Join_With_2();
    // Join::Join_With_to_String();
    // Join::rangeJoinStyle();
    // Join::Join_Get_ClassParameters();

    // Concat_1();

    // Split::Split_String_Simple();
    // Split::Split_Non_String();
    // Split::Chunk_Fixed_Size_Batches();
    // Split::Chunk_Processing_Network_Packets();
    // Split::Chunk_By_Dynamic_Grouping();
    // Split::Chunk_By_Extracting_Sentences_from_Text();

    // Slide::Simple_Example();

    // Views::Zip();
    // Views::Repeat();
    // Views::Elements();
    // Views::Enumerate();

    // Algorithms::For_Each();
    // Algorithms::Find_IF();
    // Algorithms::Find_byName();
    // Algorithms::Unique();
    // Algorithms::Sort();
    // Algorithms::Sort_ByID();
    // Algorithms::Sort_BackWards();

    // Ranges_Sort::Sort_Custom_Type();

    // Max_Element::Find_Max_Element_by_Class_Field();

    // Algorithms::Reverse();
    // Algorithms::Reverse_Views();
    // Algorithms::Reverse_Span_Part();

    // Filters::Filter_Numbers();
    // Filters::Filter_Numbers_2();
    // Filters::Filter_ForEach();
    // Filters::PrintRange();
    // Filters::Pass_Range_to_the_Function__Predicate();

    // Keys_Values_of_Map::getAllKeysFromMap();
    // Keys_Values_of_Map::getAllValuesFromMap();

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
    // Containers_From_Ranges::CreateVectorFromRange();
    // Containers_From_Ranges::CreateVectorFromRange_Inplace();

    // Experiments();

    Files::parsing_Input_Files();
}
