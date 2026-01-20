/**============================================================================
Name        : StaticString.cpp
Created on  : 19.01.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StaticString.cpp
============================================================================**/

#include "StaticString.hpp"

#include <array>
#include <string_view>
#include <algorithm>
#include <cstdint>

namespace static_string
{
    template <size_t Size>
    struct FixedString
    {
        static_assert(Size > 0, "Size must be greater than zero!");

        using value_type = char;
        using size_type  = size_t;

        std::array<value_type, Size> buffer = [] {
            std::array<value_type, Size> tmp {};
            std::fill(tmp.begin(), tmp.end(), '\0');
            return tmp;
        }();

        explicit consteval FixedString(const value_type (&str)[Size + 1])
        {
            std::copy_n(str, Size, std::begin(buffer));
        }

        explicit consteval FixedString(const std::string_view sv)
        {
            std::copy_n(sv.begin(), sv.length(), std::begin(buffer));
        }

        [[nodiscard]]
        constexpr const value_type* c_str() const noexcept
        {
            return buffer.data();
        }

        [[nodiscard]]
        constexpr value_type* data() noexcept
        {
            return buffer.data();
        }

        [[nodiscard]]
        constexpr const value_type* data() const noexcept
        {
            return buffer.data();
        }

        [[nodiscard]]
        constexpr size_type size() const noexcept
        {
            return Size;
        }

        [[nodiscard]]
        constexpr size_type length() const noexcept
        {
            size_type i = 0;
            while (i < Size && buffer[i] != '\0')
                ++i;
            return i;
        }

        [[nodiscard]]
        constexpr std::string_view view() const noexcept
        {
            return std::string_view(data(), length());
        }

        [[nodiscard]]
        explicit constexpr operator std::string_view() const noexcept
        {
            return view();
        }

        [[nodiscard]]
        constexpr value_type operator[](size_type idx) const noexcept
        {
            return buffer[idx];
        }

        [[nodiscard]]
        constexpr value_type& operator[](size_type idx) noexcept
        {
            return buffer[idx];
        }

        [[nodiscard]]
        constexpr bool starts_with(std::string_view prefix) const noexcept
        {
            if (prefix.size() > length())
                return false;

            for (size_type idx = 0; idx < prefix.size(); ++idx)
            {
                if (buffer[idx] != prefix[idx])
                    return false;
            }
            return true;
        }

        [[nodiscard]]
        constexpr bool ends_with(std::string_view suffix) const noexcept
        {
            const size_type len = length();
            if (suffix.size() > len)
                return false;

            const size_type offset = len - suffix.size();
            for (size_type idx = 0; idx < suffix.size(); ++idx)
            {
                if (buffer[offset + idx] != suffix[idx])
                    return false;
            }
            return true;
        }

        template <size_type OtherSize>
        [[nodiscard]]
        constexpr bool starts_with(const FixedString<OtherSize>& other) const noexcept
        {
            return starts_with(other.view());
        }

        template <size_type OtherSize>
        [[nodiscard]]
        constexpr bool ends_with(const FixedString<OtherSize>& other) const noexcept
        {
            return ends_with(other.view());
        }

        [[nodiscard]]
        consteval std::uint64_t hash() const noexcept
        {
            static constexpr uint64_t fnv_offset = 14695981039346656037ull;
            static constexpr uint64_t fnv_prime  = 1099511628211ull;

            std::uint64_t h = fnv_offset;
            for (size_type idx = 0; idx < length(); ++idx)
            {
                h ^= static_cast<std::uint8_t>(buffer[idx]);
                h *= fnv_prime;
            }
            return h;
        }


        friend constexpr bool operator==(const FixedString&, const FixedString&) = default;
        friend constexpr auto operator<=>(const FixedString&, const FixedString&) = default;
    };
}

namespace static_string::tests
{
    using namespace std::string_view_literals;

    consteval void test_construction_from_literal_exact_size()
    {
        constexpr FixedString<3> fs("abc");

        static_assert(fs.size() == 3);
        static_assert(fs.length() == 3);
        static_assert(fs[0] == 'a');
        static_assert(fs[1] == 'b');
        static_assert(fs[2] == 'c');
    }

    consteval void test_construction_from_string_view_shorter()
    {
        constexpr FixedString<8> fs(std::string_view("abc"));

        static_assert(fs.size() == 8);
        static_assert(fs.length() == 3);
        static_assert(fs.view() == "abc");
        static_assert(fs[3] == '\0');
    }

    void test_c_str_and_data_access()
    {
        constexpr FixedString<4> fs("abc");

        static_assert(fs.c_str()[0] == 'a');
        static_assert(fs.data()[1] == 'b');
        static_assert(fs.data()[2] == 'c');
        static_assert(fs.data()[3] == '\0');
    }

    consteval void test_view_and_string_view_conversion()
    {
        constexpr FixedString<4> fs("abc");

        static_assert(fs.view() == std::string_view("abc"));
        static_assert(std::string_view(fs) == "abc");
    }

    consteval void test_string_view_conversion()
    {
        constexpr FixedString<5> fs("abcd");

        static_assert(fs.view() == std::string_view("abcd"));
        static_assert(static_cast<std::string_view>(fs) == "abcd");
    }

    consteval void test_equality_and_ordering()
    {
        constexpr FixedString<3> a("abc");
        constexpr FixedString<3> b("abc");
        constexpr FixedString<3> c("abd");

        static_assert(a == b);
        static_assert(a != c);
        static_assert(a < c);
    }

    consteval void test_starts_with()
    {
        constexpr FixedString<16> fs("market.data");

        static_assert(fs.starts_with("market"));
        static_assert(fs.starts_with("market."));
        static_assert(fs.starts_with("market.data"));

        static_assert(!fs.starts_with("data"));
        static_assert(!fs.starts_with("market.data.extra"));
    }

    consteval void test_starts_with_fixed_string()
    {
        constexpr FixedString<16> fs("order.book");
        constexpr FixedString<5>  p1("order");
        constexpr FixedString<6>  p2("book");

        static_assert(fs.starts_with(p1));
        static_assert(!fs.starts_with(p2));
    }

    consteval void test_ends_with()
    {
        constexpr FixedString<16> fs("market.data");

        static_assert(fs.ends_with("data"));
        static_assert(fs.ends_with(".data"));
        static_assert(fs.ends_with("market.data"));

        static_assert(!fs.ends_with("market"));
        static_assert(!fs.ends_with("xxx"));
    }

    consteval void test_ends_with_fixed_string()
    {
        constexpr FixedString<16> fs("trade.update");
        constexpr FixedString<6>  s1("update");
        constexpr FixedString<5>  s2("trade");

        static_assert(fs.ends_with(s1));
        static_assert(!fs.ends_with(s2));
    }

    consteval void test_hash_equality_and_difference()
    {
        constexpr FixedString<3> a("abc");
        constexpr FixedString<3> b("abc");
        constexpr FixedString<3> c("abd");

        static_assert(a.hash() == b.hash());
        static_assert(a.hash() != c.hash());
    }

    consteval void test_hash_stability_across_construction()
    {
        constexpr FixedString<3> a("abc");
        constexpr FixedString<3> b(std::string_view("abc"));

        static_assert(a.hash() == b.hash());
    }

    consteval void test_type_properties()
    {
        static_assert(std::is_trivially_copyable_v<FixedString<3>>);
        static_assert(std::is_standard_layout_v<FixedString<3>>);
    }

    template <FixedString Name>
    struct Tag
    {
        static constexpr auto value = Name;
    };

    template <FixedString Name>
    struct TagEx
    {
        static constexpr auto value = Name;
        static constexpr auto id    = Name.hash();
    };


    consteval void test_nttp_usage()
    {
        using T = Tag<FixedString<5>("hello")>;
        static_assert(T::value == FixedString<5>("hello"));
        static_assert(T::value.hash() == FixedString<5>("hello").hash());
    }

    consteval void test_type_traits()
    {
        static_assert(std::is_trivially_copyable_v<FixedString<8>>);
        static_assert(std::is_standard_layout_v<FixedString<8>>);
    }

    consteval void test_nttp_usage_and_hash()
    {
        using T = TagEx<FixedString<5>("hello")>;
        static_assert(T::value.view() == "hello");
        static_assert(T::id == FixedString<5>("hello").hash());
    }
}

void static_string::TestAll()
{
    tests::test_construction_from_literal_exact_size();
    tests::test_construction_from_string_view_shorter();
    tests::test_c_str_and_data_access();
    tests::test_string_view_conversion();
    tests::test_view_and_string_view_conversion();
    tests::test_equality_and_ordering();
    tests::test_starts_with();
    tests::test_starts_with_fixed_string();
    tests::test_ends_with();
    tests::test_ends_with_fixed_string();
    tests::test_hash_equality_and_difference();
    tests::test_hash_stability_across_construction();
    tests::test_type_properties();
    tests::test_type_traits();
    tests::test_nttp_usage();
    tests::test_nttp_usage_and_hash();
}
