//============================================================================
// Name        : Span.cpp
// Created on  : 01.11.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Span src
//============================================================================

#include "Span.h"

#include <iostream>
#include <string>
#include <string_view>
#include <algorithm>

#include <array>
#include <span>
#include <list>
#include <vector>

namespace Span {

    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const std::span<T>& span) {
        for (const auto& entry : span)
            stream << ' ' << entry;
        return stream;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vector) {
        for (const auto& entry : vector)
            stream << ' ' << entry;
        return stream;
    }

    template<class T, std::size_t N, std::size_t M> 
    [[nodiscard]]
    constexpr bool contains(std::span<T, N> span, std::span<T, M> sub) {
        return std::search(span.begin(), span.end(), sub.begin(), sub.end())
            != span.end();
    }

    ///////////////////////////////////////////////////////////////////////////////////////

    //template<typename T>
    //void print(std::span<T> const data)  {
    void print(const std::span<const int> data) {
        for (auto offset{ 0U }; offset != data.size(); ++offset) {
            std::cout << data.subspan(offset).front() << ' ';
        }
        std::cout << '\n';
    }

    void Subspan_Test()
    {
        constexpr int data[]{ 0, 1, 2, 3, 4, 5, 6 };
        print({ data, 4 });
    }

    ///////////////////////////////////////////////////////////////

    void Create() {
        std::vector<int> numbers{ 1,2,3,4,5};
        const std::vector<int> cnumbers{ 1,2,3,4,5};
        constexpr int array[]{ 0, 1, 2, 3, 4, 5};

        std::cout << "------------------------------- 1 ---------------------------------------\n";
        {
            std::span<int> sp(numbers);
            std::cout << sp << std::endl;
        }

        std::cout << "------------------------------- 2 ---------------------------------------\n";
        {
            std::span<const int> sp(cnumbers);
            std::cout << sp << std::endl;
        }

        std::cout << "------------------------------- 3 ---------------------------------------\n";
        {
            std::span<const int> sp(numbers.data(), 3);
            std::cout << sp << std::endl;
        }

        std::cout << "------------------------------- 4 ---------------------------------------\n";
        {
            std::span<const int> sp(array);
            std::cout << sp << std::endl;
        }

        std::cout << "------------------------------- 5 ---------------------------------------\n";
        {
            std::span<const int> sp(array, std::size(array)/2);
            std::cout << sp << std::endl;
        }
    }

    void Create_Not() {
#if 0 
        std::list<int> numbers{ 1,2,3,4,5,6,7,8,9 };
        std::span<int> sp(numbers);
#endif
    }

 
    void Front() {
        constexpr int data[]{1, 2, 3, 4, 5, 6 };

        auto printFront = [](const std::span<const int> span) {
            std::cout << span.front() << std::endl;
        };

        std::span<const int> sp(data);

        std::cout << sp.front() << std::endl;
        printFront(data);
    }

    void First_Substring() {
        constexpr int data[]{ 1, 2, 3, 4, 5, 6 };
        std::span<const int> sp(data);
        std::cout << "Source collection: " << sp << "\n\n";

        for (auto i : data) {
            auto s = sp.first(i);
            std::cout << "first (" << i << ") = " << s << std::endl;
        }
    }

    void Back() {
        constexpr int data[]{ 1, 2, 3, 4, 5, 6 };
        std::span<const int> sp(data);
        std::cout << sp.back() << std::endl;
    }

    void Last() {
        constexpr int data[]{ 1, 2, 3, 4, 5, 6 };
        std::span<const int> sp(data);
        std::cout << "Source collection: " << sp << "\n\n";

        for (auto i : data) {
            auto s = sp.last(i);
            std::cout << "last (" << i << ") = " << s << std::endl;
        }

        std::cout << "-------------------  dynamic_extent  --------------------------" << std::endl;

        for (auto i : data) {
            std::span<const int, std::dynamic_extent> s = sp.last(i);
            std::cout << "last (" << i << ") = " << s << std::endl;
        }

    }

    void Subspan()
    {
        constexpr int data[]{ 1, 2, 3, 4, 5, 6 };
        std::span<const int> data_span(data);

        {
            auto sub = data_span.subspan(2, 2);
            std::cout << "subspan(2, 2) = " << sub << std::endl;
        }
    }

    void Size() {
        std::array<int, 5> data{ 1,2,3,4,5 };
        std::span<int> data_span(data);

        std::cout << data_span << std::endl;
        std::cout << "size = " << data_span.size() << std::endl;
        std::cout << "size bytext = " << data_span.size_bytes() << std::endl;
    }

    ///////////////////////////////////////////////// 

    void Handle_As_Span(const std::span<int> data) {

        std::cout << "size_bytes = " << data.size_bytes() << std::endl;

        std::cout << "Collection data (Inordered): [ ";
        std::for_each(data.begin(), data.end(), [](const auto& v) { std::cout << v << " "; });
        std::cout << "]" << std::endl;

        std::cout << "Collection data (Reversed) : [ ";
        std::for_each(data.rbegin(), data.rend(), [](const auto& v) { std::cout << v << " "; });
        std::cout << "]" << std::endl;

    }

    void Pass_Collection_As_Span() {
        {
            std::vector<int> data{ 1,2,3,4,5 };
            Handle_As_Span(data);
        }

        {
            std::list<int> data{ 1,2,3,4,5 };
            // ERROR
            // Handle_As_Span(data);
        }

        {
            std::array<int, 5> data{ 1,2,3,4,5 };
            Handle_As_Span(data);
        }

        {
            int data[]{1,2,3,4,5};
            Handle_As_Span(data);
        }
    }

    ///////////////////////////////////////////////// 

    void printX(const std::span<int> data) {

        for (const auto& v : data) {
            std::cout << v << std::endl;
        }
    }

    void _Tests_() {
        constexpr int data[]{ 0, 1, 2, 3, 4, 5, 6 };

        std::span<const int> sp(data, 4);

        std::cout << sp << std::endl;

    }

}

void Span::TestAll()
{
    // Subspan_Test();
    Subspan();

    // Create();
    // Create_Not();

    // Front();
     
    // First_Substring();

    // Back();
    // Last();

    // Size();

    // Pass_Collection_As_Span();

    // _Tests_();
}