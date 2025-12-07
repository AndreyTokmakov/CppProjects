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
#include <string>
#include <unordered_map>
#include <ranges>

// #include <experimental/meta>

// https://godbolt.org/z/6Prs3fT1b

#if 0
// ==============================================================================================================================
// https://medium.com/@massimiliano.bastia92/c-static-reflection-an-overview-of-the-metaprogramming-paradigm-shift-4cc2ca49a2c6
// ==============================================================================================================================

struct User
{
    std::string name;
    int age;
};

constexpr int max_connections = 100;

void say_hello() { }


void reflectTypes_Simple()
{
    // Reflect the struct type 'User'
   constexpr std::meta::info user_reflection = ^^User;

   // Reflect the global variable 'max_connections'
   constexpr std::meta::info connections_reflection = ^^max_connections;

   // Reflect the free function 'say_hello'
   constexpr std::meta::info hello_reflection = ^^say_hello;

   // Reflect the data member 'age' inside struct 'User'
   constexpr std::meta::info user_age_reflection = ^^User::age;
}

// The [: :] Splicer
// Once an entity has been reflected into a std::meta::info value, it can be reintroduced into the program’s code using the splicer syntax [: :].
// The splicer effectively injects a reflection back into the source code at compile time,
//    allowing the program to transform meta-level representations into real code.
// The compiler then processes the resulting code as if it had been written manually from the beginning,
//    ensuring smooth integration between the domain of reflection and the ordinary structure of the code.
// In this sense, the splicer completes the transformation initiated by reification, allowing meta-level representations produced through
//    reflection to materialize back into ordinary C++ code, ready to be compiled and executed like any other part of the program.


// The operand inside the splicer must be a constant expression that either is, or can be implicitly converted to a std::meta::info value.
// Depending on the context, splicing behaves differently:
//      [: r :]          -  produces an expression evaluating to the entity represented by r;
//      typename [: r :] -  produces a simple type specifier if r reflects a type;
//      template [: r :] -  produces a template name if r reflects a template.
//      [: r :]::        -  produces a nested-name-specifier corresponding to a namespace, enumeration type, or class.

void useReflectedData()
{
    constexpr std::meta::info int_reflection = ^^int;
    constexpr std::meta::info double_reflection = ^^double;
    constexpr std::meta::info string_reflection = ^^std::string;

    typename [: int_reflection :] number = 100;
    typename [: double_reflection :] pi = 3.14;
    typename [: string_reflection :] message = "Hello Reflection";

    std::cout << "Number: " << number << std::endl;
    std::cout << "Pi: " << pi << std::endl;
    std::cout << "Message: " << message << std::endl;

    // Number: 100
    // Pi: 3.14
    // Message: Hello Reflection
}


template <typename T>
void splicing_example()
{
    constexpr auto vec = ^^std::vector<T>;
    typename [: vec :] x = {1, 2, 3};
    for (const auto& elem : x) {
        std::cout << elem << " ";
    }

    // Output: 1 2 3
}

void retrieve_Data_Members_Names()
{
    // Reflect the type User
    constexpr std::meta::info user_reflection = ^^User;

    // Retrieve the name of the reflected type
    std::cout << "Type: " << std::meta::identifier_of(user_reflection) << std::endl;

    // Reflect and list the non-static data members
    [: expand(std::meta::nonstatic_data_members_of(user_reflection)) :] >>
    [&]<std::meta::info member>() {
        std::cout << " Member: " << std::meta::identifier_of(member) << std::endl;
    };

    // Type: User
    //  Member: name
    //  Member: age
}

// A simple generic vector with 3 elements of type T
template <typename T>
struct MyVec
{
    std::array<T, 3> data;
};

// A simple map-like struct with a key and a value
template <typename Key, typename Value>
struct MyMap
{
    Key key;
    Value value;
};


void create_new_type()
{
    // Step 1: Reflect the individual types we'll need
    constexpr std::meta::info key_info = ^^int;        // Get meta info for 'int'
    constexpr std::meta::info vec_template = ^^MyVec;  // Get meta info for the template 'MyVec'
    constexpr std::meta::info value_arg = ^^double;    // Get meta info for 'double'

    // Step 2: Use reflection to create the type MyVec<double>
    // This "fills in" the template MyVec<T> with T = double
    constexpr std::meta::info vec_info = std::meta::substitute(vec_template, {value_arg});

    // Step 3: Use reflection to create the type MyMap<int, MyVec<double>>
    //  This creates a reflected instantiation of MyMap with the previous types
    constexpr std::meta::info map_info = std::meta::substitute(^^MyMap, {key_info, vec_info});

    // Step 4: Splice the reflected type into real C++ code using [: :]
    // This gives us a real usable type: MyMap<int, MyVec<double>>
    typename [:map_info:] obj = {
        7,                              // obj.key = 7
        { {1.1, 2.2, 3.3} }             // obj.value = MyVec<double> with 3 doubles
    };

    // Step 5: Use the object like any normal type
    std::cout << "Key: " << obj.key << "\nValues:\n";
    for (double v : obj.value.data)  // Loop through the MyVec's data array
        std::cout << v << '\n';

    // EDITED ON June 21, 2025 for better clarity
}


int main()
{
    reflectTypes_Simple();

    useReflectedData();

    splicing_example<int>();

    retrieve_Data_Members_Names();

    create_new_type();
}

// ==============================================================================================================================
// ==============================================================================================================================

#endif


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
