/**============================================================================
Name        : Hashing_Utilities.cpp
Created on  : 16.08.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Hashing_Utilities
============================================================================**/

#include "Hashing.hpp"

#include <unordered_set>
#include <string>
#include <iostream>
#include <print>
#include <optional>

template<typename T>
concept is_std_hashable = requires(const T& object) {
    {std::hash<T>{}(object)} -> std::convertible_to<std::size_t>;
};

template<typename T>
concept has_hash_member_function = requires(const T& object) {
    {object.hash()} -> std::convertible_to<std::size_t>;
};

template<typename T>
concept hashable = is_std_hashable<T> || has_hash_member_function<T>;

template<typename T>
concept is_iterable = requires(const T& object)
{
    object.begin();
    object.end();
};

template<hashable T>
std::size_t hash(const T& object)
{
    if constexpr(is_std_hashable<T>)
        return std::hash<T>{}(object);
    else
        return object.hash();
}

template <class T>
void hash_combine(std::size_t& seed, const T& v) {
    seed ^= hash(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

template<hashable ...Ts>
std::size_t hash(const Ts& ...objects)
{
    std::size_t result = 0;
    (hash_combine(result, objects), ...);
    return result;
}

template<has_hash_member_function T>
struct std::hash<T>
{
    std::size_t operator()(const T& object) const noexcept {
        return object.hash();
    }
};

template<is_iterable Iterable>
struct std::hash<Iterable>
{
    std::size_t operator()(const Iterable& range) const noexcept
    {
        std::size_t result = 0;
        for (const auto& object: range)
            hash_combine(result, object);
        return result;
    }
};

template<typename T1, typename T2>
struct std::hash<std::pair<T1, T2>>
{
    std::size_t operator()(const std::pair<T1, T2>& pair) const noexcept {
        return ::hash(pair.first, pair.second);
    }
};

template<typename ...Ts>
struct std::hash<std::tuple<Ts...>>
{
    std::size_t operator()(const std::tuple<Ts...>& tuple) const noexcept
    {
        auto to_apply = [](const auto& ...objects) { return ::hash(objects...); };
        return std::apply(to_apply, tuple);
    }
};

// user side
struct Person
{
    bool operator==(const Person&) const = default;

    [[nodiscard]]
    std::size_t hash() const noexcept
    {
        return ::hash(name, age);
    }

    std::string name;
    int age;
};

void Hashing::TestUtilities()
{
    std::unordered_set<std::optional<Person>> persons;
    persons.emplace(Person{"Antoine", 33});
    persons.emplace(Person{"Cedric", 30});
    persons.emplace(Person{"Antoine", 30});
    persons.emplace(Person{"Vasyl", 29});

    for(const auto &person: persons)
    {
        std::println("{}: {}", person->name, person->age);
    }

    std::cout << hash(persons) << std::endl;

    const auto pair = std::make_pair(1, persons);
    const auto tuple = std::make_tuple(1, std::string("Antoine"), persons);

    std::cout << hash(pair) << " " << hash(tuple) << std::endl;

    /**
    Vasyl: 29
    Antoine: 30
    Cedric: 30
    Antoine: 33
    16394850612421563443
    16394850788073546592 6502939163776857393
    **/
}
