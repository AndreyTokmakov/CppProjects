/**============================================================================
Name        : Span.cpp
Created on  : 01.11.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Span src
============================================================================**/

#include "Span.h"

#include <iostream>
#include <string>
#include <string_view>
#include <algorithm>

#include <array>
#include <span>
#include <list>
#include <vector>
#include <print>
#include <cstring>

namespace Span
{
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


    void Create()
    {
        std::vector<int> numbers{ 1,2,3,4,5};
        const std::vector<int> numSpan{ 1,2,3,4,5};
        constexpr int array[]{ 0, 1, 2, 3, 4, 5};

        std::cout << "------------------------------- 1 ---------------------------------------\n";
        {
            const std::span<int> sp(numbers);
            std::cout << sp << std::endl;
        }

        std::cout << "------------------------------- 2 ---------------------------------------\n";
        {
            const std::span<const int> sp(numSpan);
            std::cout << sp << std::endl;
        }

        std::cout << "------------------------------- 3 ---------------------------------------\n";
        {
            const std::span<const int> sp(numbers.data(), 3);
            std::cout << sp << std::endl;
        }

        std::cout << "------------------------------- 4 ---------------------------------------\n";
        {
            const std::span<const int> sp(array);
            std::cout << sp << std::endl;
        }

        std::cout << "------------------------------- 5 ---------------------------------------\n";
        {
            const std::span<const int> sp(array, std::size(array)/2);
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
        const std::span<const int> sp(data);
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
        const std::span<const int> sp(data);
        std::cout << "Source collection: " << sp << "\n\n";

        for (const int i : data) {
            auto s = sp.last(i);
            std::cout << "last (" << i << ") = " << s << std::endl;
        }

        std::cout << "-------------------  dynamic_extent  --------------------------" << std::endl;

        for (const auto i : data) {
            std::span<const int, std::dynamic_extent> s = sp.last(i);
            std::cout << "last (" << i << ") = " << s << std::endl;
        }

    }

    void Subspan()
    {
        constexpr int data[]{ 1, 2, 3, 4, 5, 6 };
        std::span<const int> data_span(data);

        {
            const auto sub = data_span.subspan(2, 2);
            std::cout << "subspan(2, 2) = " << sub << std::endl;
        }
        {
            std::vector<int> values (std::begin(data), std::end(data));
            values.erase(values.begin() + 2);
            std::cout << values << std::endl;
        }
    }

    void Size()
    {
        std::array<int, 5> data{ 1,2,3,4,5 };
        const std::span<int> data_span(data);

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

namespace Span::StaticSize
{

    size_t fn1(std::span<int> data) {
        size_t result = 0;
        for (size_t i = 0; i < data.size(); ++i)
            result += i;
        return result;
    }

    size_t fn2(std::span<int, 1024> data) {
        size_t result = 0;
        for (size_t i = 0; i < data.size(); ++i)
            result += i;
        return result;
    }

    void Static_Sized_Array()
    {
        std::array<int, 1024> data1 {};
        // std::span arr1 = data1;

        // decltype(arr1) == std::span<int, 1024>
        // static_assert(std::is_same_v<decltype(arr1), std::span<int, 1024>>);
        fn2(data1);

        int data2[1024];

        // std::span arr2 = data2;
        // delctype(arr2) == std::span<int, 1024>
        // static_assert(std::is_same_v<decltype(arr2), std::span<int, 1024>>);
        fn2(data2);

        std::vector<int> data3(1024);
        std::span arr3 = data2;
        fn1(data3); // OK
        // fn2(data3); // Wouldn't compile
    }
}

namespace Span::SizeOf_Span
{
    void SizeOf_Creation()
    {
        int arr[] = {1, 2, 3, 4, 5};
        std::vector<int> vec = {1, 2, 3, 4, 5};
        std::span<int, 5> arr_span {arr};
        std::span<int> other_span {arr};
        std::span<int> vec_span{vec};

        std::cout << std::format("sizeof arr_span: {}\n", sizeof(arr_span));
        std::cout << std::format("sizeof other_span: {}\n", sizeof(other_span));
        std::cout << std::format("sizeof vec_span: {}\n", sizeof(vec_span));

        /** sizeof arr_span: 8
            sizeof other_span: 16
            sizeof vec_span: 16
        **/
    }
}

namespace Handle_Static_Dynamic_Content
{
    // Templated function to handle both dynamic and static extent spans
    template<typename T, std::size_t Extent>
    void printSpan(std::span<const T, Extent> numbers)
    {
        // Compile-time check to distinguish dynamic vs static extent
        if constexpr (Extent == std::dynamic_extent) {
            std::print("Dynamic extent with size {} | sizeof(span)={} \n", numbers.size(), sizeof(numbers));
        } else {
            std::print("Static extent={} | size={} | sizeof(span)={} \n", Extent, numbers.size(), sizeof(numbers));
        }

        // Create a subspan from index 1, length 3
        std::span<const int> sub = numbers.subspan(1,3);
        std::print("Subspan: {}\n", sub);

        // Show first 2 elements of the subspan
        std::print("First 2: {}\n", sub.first(2));

        // Show last 2 elements of the subspan
        std::print("Last 2: {}\n\n", sub.last(2));
    }

    void print()
    {
        const std::vector<int> vecInt { 1, 2, 3, 4, 5 };
        const std::array<int,4> arrInt { 10, 20, 30, 40 };
        const int rawInt[3] {100, 200, 300};

        std::span<const int> vecSpan{vecInt};      // dynamic extent, const
        std::span<const int> arrSpan{arrInt};       // dynamic extent, const
        std::span<const int> rawSpan{rawInt};       // dynamic extent, const
        std::span<const int,3> staticSpan{rawInt};  // static extent, const

        printSpan(vecSpan);
        printSpan(arrSpan);
        printSpan(rawSpan);
        printSpan(staticSpan);

        // Dynamic extent with size 5 | sizeof(span)=16
        // Subspan: [2, 3, 4]
        // First 2: [2, 3]
        // Last 2: [3, 4]
        //
        // Dynamic extent with size 4 | sizeof(span)=16
        // Subspan: [20, 30, 40]
        // First 2: [20, 30]
        // Last 2: [30, 40]
        //
        // Dynamic extent with size 3 | sizeof(span)=16
        // Subspan: [200, 300, 32765]
        // First 2: [200, 300]
        // Last 2: [300, 32765]
        //
        // Static extent=3 | size=3 | sizeof(span)=8
        // Subspan: [200, 300, 32765]
        // First 2: [200, 300]
        // Last 2: [300, 32765]
    }
}

namespace Parse_Network_Data
{
    struct ChunkHeader
    {
        uint8_t type;   // Chunk type identifier
        uint8_t length; // Length of the data payload
    };

    // Parse a chunk and return how many bytes were consumed (header + payload)
    size_t parse_chunk(std::span<const uint8_t> data)
    {
        if (data.size() < sizeof(ChunkHeader))
            return 0; // Not enough data for header

        // Ensure ChunkHeader is trivial before using memcpy
        static_assert(std::is_trivial_v<ChunkHeader>, "ChunkHeader must be trivial");

        // Safe copy to avoid strict aliasing issues
        ChunkHeader header {};
        std::memcpy(&header, data.data(), sizeof(ChunkHeader));

        // Ensure payload fits within available data
        if (data.size() < sizeof(ChunkHeader) + header.length) return 0;

        // Create a subspan representing the payload
        auto payload = data.subspan(sizeof(ChunkHeader), header.length);

        // Print chunk info
        std::print("Chunk type {} length {}: ", header.type, header.length);
        for (auto b : payload)
            std::print("{} ", b);
        std::print("\n");

        // Return total bytes consumed so the caller can advance the span
        return sizeof(ChunkHeader) + header.length;
    }

    void parseTest()
    {
        // Example file data
        std::vector<uint8_t> file = {1, 3, 10, 11, 12, 2, 2, 99, 100};
        std::span<const uint8_t> file_span(file);

        // Iterate over chunks using span, no raw pointer arithmetic needed
        while (!file_span.empty())
        {
            size_t consumed = parse_chunk(file_span);
            if (consumed == 0)
                break; // Stop if not enough data left
            file_span = file_span.subspan(consumed); // Advance span safely
        }
    }
}


void Span::TestAll()
{
    // Subspan_Test();
    // Subspan();

    // Create();
    // Create_Not();

    // Front();
     
    // First_Substring();

    // Back();
    // Last();

    // SizeOf_Span::SizeOf_Creation();
    // Size();

    // Pass_Collection_As_Span();

    // _Tests_();

    // StaticSize::Static_Sized_Array();

    // Handle_Static_Dynamic_Content::print();

    Parse_Network_Data::parseTest();
}