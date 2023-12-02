/**============================================================================
Name        : Optional.cpp
Created on  : 16.07.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Optional
============================================================================**/

#include "Optional.h"

#include <utility>
#include <optional>
#include <array>
#include <algorithm>
#include <unordered_map>
#include <format>
#include "../Helpers/Utilities.h"

namespace Optional
{
    template<typename _Ty>
    struct MyOptional
    {
        using data_type = _Ty;
        using pointer   = data_type*;

    private:
        // using aligned_storage_t = std::aligned_storage_t<sizeof(data_type), alignof(data_type)>;
        // aligned_storage_t data;

        // char data [sizeof(data_type)]{};
        std::array<u_int8_t, sizeof(data_type)> storage {};
        bool has_value { false };


    public:
        MyOptional() noexcept = default;

        template<typename ... Types>
        MyOptional(Types&& ... params) {
            ::new (storage.data()) data_type(std::forward<Types>(params)...);
            has_value = true;
        }

        [[nodiscard]]
        pointer asPointer() noexcept {
            return reinterpret_cast<pointer>(storage.data());
        }

        inline explicit operator bool() const noexcept {
            return has_value;
        }

        void destroy()
        {
            if (has_value) {
                asPointer()->~data_type();
            }
        }

        [[nodiscard]]
        inline bool hasValue() const noexcept {
            return has_value;
        }

        void set(_Ty&& newVal)
        {
            destroy();

            pointer objPtr = reinterpret_cast<pointer>(storage.data());
            std::exchange(*objPtr, std::forward<data_type>(newVal));

            has_value = true;
        }

        ~MyOptional() {
            destroy();
        }
    };
}

namespace Cpp23_Features
{
    const std::unordered_map<int, std::string> cache {
            {1, "I"},
            {2, "II"},
            {3, "III"},
            {4, "IV"},
            {5, "V"},
    };

    const std::unordered_map<int, std::string> database {
            {1, "I"},
            {2, "II"},
            {3, "III"},
            {4, "IV"},
            {5, "V"},
            {6, "VI"},
            {7, "VII"},
            {8, "VIII"},
            {9, "IX"},
            {10, "X"},
    };

    std::optional<std::string> getFromCache(int key)
    {
        const auto iter = cache.find(key);
        if (cache.end() != iter)
            return std::make_optional<std::string>(iter->second);
        return std::nullopt;
    }

    std::optional<std::string> getFromDatabase(int key)
    {
        const auto iter = database.find(key);
        if (database.end() != iter)
            return std::make_optional<std::string>(iter->second);
        return std::nullopt;
    }

    std::optional<std::string> decorate(const std::string& str)
    {
        return std::format("[{}]", str);
    }

    std::string toUpper(const std::string& str) {
        std::string tmp {str};
        std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
        return tmp;
    }

    template<typename _Ty>
    std::ostream& operator<<(std::ostream& stream, const std::optional<_Ty>& opt)
    {
        if (opt.has_value())
            stream << opt.value();
        else
            stream <<  "NullOpt";
        return stream;
    }

    std::optional<int> getAge(int v)
    {
        if (100 > v)
            return std::make_optional<int>(v);
        return std::nullopt;
    }

    void OrElse_Test()
    {
        for (int val: {3, 7})
        {
            const std::optional<std::string> result = getFromCache(val)
                    .or_else([&]() { return getFromDatabase(val); });

            std::cout << val << " --> " << result << std::endl;
        }

        std::cout << std::endl;

        for (int val: {3, 7, 13})
        {
            const std::optional<std::string> result = getFromCache(val)
                    .or_else([&]() { return getFromDatabase(val); })
                    .or_else([&]() { return std::make_optional<std::string>("None"); });

            std::cout << val << " --> " << result << std::endl;
        }
    }

    void AndThen_Test()
    {
        for (int val: {3, 7})
        {
            const std::optional<std::string> result = getFromCache(val)
                    .and_then(decorate);
            std::cout << val << " --> " << result << std::endl;
        }

        for (int val: {3, 7})
        {
            const std::optional<std::string> result = getFromCache(val)
                    .and_then([](const std::string& str) { return std::make_optional<std::string>("OK"); });
            std::cout << val << " --> " << result << std::endl;
        }
    }

    void Transform_Test()
    {
        {
            const std::optional<int> number = 5;
            const std::optional<int> squared = number.transform([](int x) { return x * x; });
            std::cout << number << " --> " << squared << std::endl;
        }

        {
            const std::optional<std::string> text = "qwerty";
            const std::optional<std::string> upperCase = text.transform(toUpper);
            std::cout << text << " --> " << upperCase << std::endl;
        }
    }
}

void Optional::TestAll()
{
    using namespace Helpers;

    /*
    MyOptional<Long> opt;
    opt.set(Long {2});
    */


    // Cpp23_Features::OrElse_Test();
    // Cpp23_Features::AndThen_Test();
    // Cpp23_Features::Transform_Test();

    /*

    int value = getAge(103)
            .or_else([]() { return std::make_optional<int>(18); })
            .transform([](int age) { return age + 1; }).value();


    std::cout << value << std::endl;
     */
}
