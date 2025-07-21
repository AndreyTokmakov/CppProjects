/**============================================================================
Name        : Expected.cpp
Created on  : 21.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Expected.cpp
============================================================================**/

#include "Expected.h"

#include <iostream>
#include <variant>

namespace Expected
{
    template<typename Ts, typename Err>
    struct Expected: public std::variant<Ts, Err>
    {
        template <typename... Args>
        constexpr Expected(Args&&... params) : std::variant<Ts, Err>{std::forward<Args>(params)...} {}

        [[nodiscard]]
        constexpr bool is_error() const noexcept {
            return std::holds_alternative<Err>(*this);
        }

        [[nodiscard]]
        constexpr explicit operator bool() const noexcept {
            return not is_error();
        }

        [[nodiscard]]
        constexpr bool has_value() const noexcept {
            return std::holds_alternative<Ts>(*this);
        }

        constexpr Ts& value() & noexcept {
            return std::get<Ts>(*this);
        }

        [[nodiscard]]
        constexpr const Ts& value() const & noexcept {
            return std::get<Ts>(*this);
        }

        constexpr Ts&& value() && noexcept {
            return std::move(std::get<Ts>(*this));
        }

        [[nodiscard]]
        constexpr const Ts&& value() const && noexcept {
            return std::move(std::get<Ts>(*this));
        }

        constexpr Err& error() & noexcept {
            return std::get<Err>(*this);
        }

        [[nodiscard]]
        constexpr const Err& error() const & noexcept {
            return std::get<Err>(*this);
        }

        constexpr Err&& error() && noexcept {
            return std::move(std::get<Err>(*this));
        }

        [[nodiscard]]
        constexpr const Err&& error() const && noexcept {
            return std::move(std::get<Err>(*this));
        }

        template<typename Ty = std::remove_cv_t<Ts>>
        [[nodiscard]]
        constexpr Ts value_or(Ty&& default_value) const &
        {
            if (std::holds_alternative<Ts>(*this)) {
                return std::get<Ts>(*this);
            }
            return static_cast<Ty>(std::forward<Ty>(default_value));
        }

        template<typename Ty = std::remove_cv_t<Ts>>
        [[nodiscard]]
        constexpr Ts value_or(Ty&& default_value)  &&
        {
            if (std::holds_alternative<Ts>(*this)) {
                return std::move(std::get<Ts>(*this));
            }
            return static_cast<Ty>(std::forward<Ty>(default_value));
        }

        template<typename E = std::remove_cv_t<Err>>
        [[nodiscard]]
        constexpr Err error_or(Err&& err) const &
        {
            if (std::holds_alternative<Err>(*this)) {
                return std::get<Err>(*this);
            }
            return static_cast<E>(std::forward<E>(err));
        }

        template<typename E = std::remove_cv_t<Err>>
        [[nodiscard]]
        constexpr Err error_or(E&& err) &&
        {
            if (std::holds_alternative<Err>(*this)) {
                return std::move(std::get<Err>(*this));
            }
            return static_cast<E>(std::forward<E>(err));
        }
    };
}


namespace
{
    enum class Error
    {
        Success,
        Failure
    };


    Expected::Expected<std::string, int> getData(int v)
    {
        if (100 > v && v > 0) {
            return std::to_string(v);
        }

        return { 1 };
    }
}

void Expected::TestAll()
{

    const Expected<std::string, int>  result;// = getData(143);
    if (result) {
        std::cout << "value = " << result.value() << std::endl;
    } else {
        std::cout << "error = " << result.error() << std::endl;
    }

    std::cout << "has_value = " << std::boolalpha << result.has_value() << std::endl;
    std::cout << "is_error  = " << std::boolalpha << result.is_error() << std::endl;
    std::cout << "value_or  = " << result.value_or("None") << std::endl;
    std::cout << "error_or  = " << result.error_or(0) << std::endl;


}