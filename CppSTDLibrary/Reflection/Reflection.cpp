/**============================================================================
Name        : Reflection.cpp
Created on  : 12.09.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Reflection.cpp
============================================================================**/

#include "Reflection.hpp"

#include <iostream>
#include <optional>
#include <type_traits>
#include <array>
#include <numeric>
#include <string>
#include <unordered_map>
#include <ranges>

// #include <experimental/meta>

// https://godbolt.org/z/6Prs3fT1b


#if 0

constexpr auto r = ^^int;
typename[:r:] x = 42;       // Same as: int x = 42;
typename[:^^char:] c = '*';  // Same as: char c = '*';

#endif



#if 0
namespace demo1
{
    #include <experimental/reflect>
    using namespace std::experimental::reflect;

    enum Color {
        Red,
        Green,
        Blue
    };

    using MetaT = reflexpr(Color);

    int main() {
        constexpr std::string_view name = get_name_v<get_element_t<0, get_enumerators_t<MetaT>>>;
        std::cout << "The name of the first value is \"" << name << "\"" << std::endl;
    }
}
#endif



#if 0

// compiler : clang x86-64 Reflectin-TS
// flags  : -std=c++2b -O2 -pedantic-errors

namespace demo2
{

#include <experimental/reflect>
#include <string_view>

    enum class weekdays {
        monday,
        tuesday,
        wednesday,
        thursday,
        friday,
        saturday,
        sunday
    };

    namespace meta = std::experimental::reflect;

    template <typename... MEC>
    struct enum_to_string_helper
    {
        template <typename E>
        static std::string_view find(E e) {
            return _do_find(e, MEC{}...);
        }

        template <typename E, typename ME1, typename... MEs>
        static std::string_view _do_find(E e, ME1, MEs... mes)
        {
            if (meta::get_constant_v<ME1> == e) {
                return {meta::get_name_v<ME1>};
            }
            return _do_find(e, mes...);
        }

        template <typename E>
        static std::string_view _do_find(E) {
            return {};
        }
    };

    template <typename E>
    std::string_view enum_to_string(E e) {
        return meta::unpack_sequence_t<enum_to_string_helper, meta::get_enumerators_t<reflexpr(E)>>::find(e);
    }

    void test()
    {
        std::cout << enum_to_string(weekdays::monday) << std::endl;
        std::cout << enum_to_string(weekdays::tuesday) << std::endl;
        std::cout << enum_to_string(weekdays::wednesday) << std::endl;
        std::cout << enum_to_string(weekdays::thursday) << std::endl;
        std::cout << enum_to_string(weekdays::friday) << std::endl;
        std::cout << enum_to_string(weekdays::saturday) << std::endl;
        std::cout << enum_to_string(weekdays::sunday) << std::endl;
    }

    // monday
    // tuesday
    // wednesday
    // thursday
    // friday
    // saturday
    // sunday
}
#endif

//  Still not supported bt GCC
#if 0

namespace demo3
{
    #include <experimental/meta>
    #include <experimental/compiler>
    using namespace std::experimental;

    namespace enum_util
    {
        template<typename T>
        requires std::is_enum_v<T>
        constexpr std::string_view to_string(T value)
        {
            template for (constexpr meta::info e : meta::members_of(^T)) {
                if ([:e:] == value) {
                    return meta::name_of(e);
                }
            }
            throw std::runtime_error("Unknown enum value");
        }

        template<typename T>
        requires std::is_enum_v<T>
        constexpr std::optional<T> from_string(std::string_view value)
        {
            template for (constexpr meta::info e : meta::members_of(^T)) {
                if (meta::name_of(e) == value) {
                    return [:e:];
                }
            }
            return {};
        }

    } // namespace enum_util

    enum LightColor
    {
        Red,Green, Blue
    };

    void print_name(const LightColor color)
    {
        const std::string_view str = enum_util::to_string(color);
        std::cout << "The color name is " << str << std::endl;
    }

    void test()
    {
        print_name(Red);
        print_name(Green);
        print_name(Blue);
        try {
            print_name(static_cast<LightColor>(1337));
        } catch (const std::runtime_error& e) {
            std::cout << "exception is: " << e.what() << std::endl;
        }

        static_assert(enum_util::from_string<LightColor>("Red").value() == Red);
        static_assert(enum_util::from_string<LightColor>("Green").value() == Green);
        static_assert(enum_util::from_string<LightColor>("Blue").value() == Blue);
        static_assert(not enum_util::from_string<LightColor>("Magenta").has_value());

    }
}
#endif


#if 0
namespace print_enum_values_2
{
    template<typename E>
    requires std::is_enum_v<E>
    consteval auto enumerators_of()
    {
        return []<size_t ... I>(std::index_sequence<I...>)
        {
            return std::array{[:enumerators_of(^^E)[I]:] ...};
        }
        (std::make_index_sequence<enumerators_of(^^E).size()>{});
    }

    template<typename E>
    requires std::is_enum_v<E>
    consteval auto identifiers_of()
    {
        return []<size_t ... I>(std::index_sequence<I...>)
        {
            return std::array{std::string_view(identifier_of(enumerators_of(^^E)[I])) ...};
        }
        (std::make_index_sequence<enumerators_of(^^E).size()>{});
    }

    template<typename E>
    requires std::is_enum_v<E>
    const auto& to_string(E value)
    {
        static const auto toStringMap = [] static
        {
            static constexpr auto AllValues = enumerators_of<E>();
            static constexpr auto AllStrings = identifiers_of<E>();
            static std::unordered_map<E, const std::string> result;
            for(const auto& [enumValue, string] : std::ranges::zip_view(AllValues, AllStrings))
                result.emplace(enumValue, string);
            return result;
        }();
        return toStringMap.at(value);
    }

    template<typename E>
    requires std::is_enum_v<E>
    std::ostream& operator<<(std::ostream& s, E value)
    {
        s << to_string(value);
        return s;
    }

    enum class Color { Red, Green, Blue, Yellow, Purple };
    enum class Animal { Cat, Dog, Horse, Rabbit, Snail };

    int demo()
    {
        for(const auto color : enumerators_of<Color>())
            std::cout << color << std::endl;

        std::cout << std::endl;

        for(const auto animal : enumerators_of<Animal>())
            std::cout << animal << std::endl;
    }

    /**
    Red
    Green
    Blue
    Yellow
    Purple

    Cat
    Dog
    Horse
    Rabbit
    Snail
    **/
}
#endif

void Reflection::TestAll()
{

}
