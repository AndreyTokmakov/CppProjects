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
#include <array>
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

void Optional::TestAll()
{
    using namespace Helpers;

    MyOptional<Long> opt;
    opt.set(Long {2});
}
