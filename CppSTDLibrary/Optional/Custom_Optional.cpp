/**============================================================================
Name        : Custom_Optional.cpp
Created on  : 11.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Custom_Optional.cpp
============================================================================**/

#include "Custom_Optional.h"

#include <utility>
#include <array>
#include <algorithm>
#include <format>

#include "../Helpers/Helpers.h"

namespace Custom_Optional
{
    template<typename _Ty>
    struct Optional
    {
        using data_type = _Ty;
        using pointer   = data_type*;
        using reference = data_type&;

    private:
        // using aligned_storage_t = std::aligned_storage_t<sizeof(data_type), alignof(data_type)>;
        // aligned_storage_t data;

        // char data [sizeof(data_type)]{};
        std::array<u_int8_t, sizeof(data_type)> storage {};
        bool has_value { false };


    public:
        Optional() noexcept = default;

        template<typename ... Types>
        Optional(Types&& ... params) {
            ::new (storage.data()) data_type(std::forward<Types>(params)...);
            has_value = true;
        }

        [[nodiscard]]
        pointer as_pointer() noexcept {
            return reinterpret_cast<pointer>(storage.data());
        }

        [[nodiscard]]
        reference value()  {
            return reinterpret_cast<reference>(storage);
        }

        inline explicit operator bool() const noexcept {
            return has_value;
        }

        void destroy()
        {
            if (has_value) {
                as_pointer()->~data_type();
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

        ~Optional() {
            destroy();
        }
    };
}

namespace Custom_Optional
{

    std::optional<int> getAge(int v)
    {
        if (100 > v)
            return std::make_optional<int>(v);
        return std::nullopt;
    }

}

void Custom_Optional::TestAll()
{
    using namespace Helpers;

    Optional<Long> opt(2);
    //opt.set(Long {2});

    Long& ref = opt.value();

    std::cout << ref.value << std::endl;
}