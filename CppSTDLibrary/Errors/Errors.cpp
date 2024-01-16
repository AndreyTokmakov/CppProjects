/**============================================================================
Name        : Errors.cpp
Created on  : 16.01.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Errors.cpp
============================================================================**/

#include "Errors.h"

#include <system_error>
#include <string>
#include <iostream>
#include <utility>
#include <expected>
#include <cassert>
#include <future>
#include <optional>
#include <iostream>
#include <string>
#include <string_view>
#include <cstring>
#include <fstream>
#include <functional>
#include <filesystem>
#include <tuple>
#include <ranges>
#include <cassert>

#include <exception>
#include <thread>
#include <future>
#include <mutex>
#include <syncstream>
#include <utility>

#include <numeric>
#include <queue>

namespace Errors::Basics
{
    void Make_Error_Code()
    {
        std::error_code err = std::make_error_code(std::errc::not_enough_memory);

        std::cout << err << std::endl;
        std::cout << err.category().name() << std::endl; /// "generic"
        std::cout << err.message() << std::endl;         /// "Cannot allocate memory"
        std::cout << err.value() << std::endl;           /// ENOMEM
    }

    void Construct_Error_Code()
    {
        std::error_code err = std::error_code(std::to_underlying(std::errc::not_enough_memory),
                                              std::generic_category());

        std::cout << err << std::endl;
        std::cout << err.category().name() << std::endl; /// "generic"
        std::cout << err.message() << std::endl;         /// "Cannot allocate memory"
        std::cout << err.value() << std::endl;           /// ENOMEM
    }

    void Test2()
    {
        using namespace std::string_view_literals;

        const std::error_code err = std::make_error_code(std::errc::not_enough_memory);
        const std::error_code err_e = std::error_code(std::to_underlying(std::errc::not_enough_memory),
                                                      std::generic_category());
        std::cout << ((err == err_e) ? "Equal"sv : "Different"sv) << std::endl;

        // Note that the text is implementation specific, however, specifically for std::errc the values map to errno.
        std::cout << err.category().name() << " error (" << err.value() << ") " << std::quoted(err.message()) << "\n";

        const std::error_code future = std::make_error_code(std::future_errc::promise_already_satisfied);
        // future.category().name() == "future" and err.message() == "Promise already satisfied"

        std::cout << future.category().name() << " error (" << future.value() << ") " << std::quoted(future.message()) << "\n";
    }
}

namespace Errors::CustomErrorCategory {
    /** Custom error enum : Forward declaration **/
    enum class TransactionError;
}

/** Register the enum as an error code enum **/
template<>
struct std::is_error_code_enum<Errors::CustomErrorCategory::TransactionError>:
        public std::true_type{};

namespace Errors::CustomErrorCategory
{
    // Custom error enum
    enum class TransactionError {
        OK = 0,
        TemporaryError = 1,
        PermanentError = 2,
    };

    // Custom category that provides text description
    struct TransactionErrorCategory : std::error_category
    {
        [[nodiscard]]
        const char*name() const noexcept override {
            return "transaction";
        }

        [[nodiscard]]
        std::string message( int condition ) const override
        {
            using namespace std::string_literals;
            switch(condition) {
                case 0: return "ok"s;
                case 1: return "temporary error, please retry"s;
                case 2: return "permanent error"s;
            }
            std::abort(); // unreachable
        }
    };

    // Mapping from error code enum to category
    std::error_code make_error_code(TransactionError err)
    {
        static TransactionErrorCategory category = TransactionErrorCategory{};
        return std::error_code {std::to_underlying(err), category };
    }

    // And now we can use it:
    std::error_code my_function() noexcept {
        return TransactionError::PermanentError;
    }

    // And obviously, this also works with std::expected
    std::expected<void,std::error_code> my_other_function() noexcept {
        return std::unexpected{TransactionError::TemporaryError};
    }

    void UseCustomErr()
    {
        if (auto err = my_function(); err)
        {
            std::cout << err.category().name() << " : " << err.message() << "\n";
            // prints: "transaction : permanent error"
        }

        if (auto res = my_other_function(); !res)
        {
            std::cout << res.error().category().name() << " : " << res.error().message() << "\n";
            // prints: "transaction : temporary error, please retry"
        }
    }
}




void Errors::TestAll()
{

    // Basics::Make_Error_Code();
    // Basics::Construct_Error_Code();
    // ErrorCodes::Basics::Test2();

    CustomErrorCategory::UseCustomErr();
}