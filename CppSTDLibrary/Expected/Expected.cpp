/**============================================================================
Name        : Expected.cpp
Created on  : 20.05.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Expected C++23 Library tests
============================================================================**/

#include <iostream>
#include <string_view>
#include <expected>

#include "Expected.h"

namespace Expected
{
    enum class parse_error {
        InvalidChar,
        Overflow
    };

    template<typename Ty, typename ErrorType>
    void isError(const std::expected<Ty, ErrorType> &ex) {
        if (!ex) {
            std::cout << "Contains an error value\n";
            std::cout << "Error: " << ex.error() << std::endl;
        } else
            std::cout << "OK. No error!\n";
    }

    template<typename Ty, typename ErrorType>
    void basic_functions_tests(const std::expected<Ty, ErrorType> &ex)
    {
        std::cout << "error()     : " << ex.error() << std::endl;
        std::cout << "bool()      : " << std::boolalpha<<  static_cast<bool>(ex) << std::endl;
        std::cout << "bool(not)   : " << std::boolalpha<<  static_cast<bool>(not ex) << std::endl;
        std::cout << "bool(!)     : " << std::boolalpha<<  static_cast<bool>(!ex) << std::endl;
        if (ex.has_value()) {
            std::cout << "value()     : " << ex.value() << std::endl;
            std::cout << "has_value() : " << ex.has_value() << std::endl;
        }
        std::cout << "error_or()  : " << ex.error_or(100500) << std::endl;

    }

    void BasicFunctions()
    {
        std::expected<double, int> ok = 3.14;
        basic_functions_tests(ok);

        std::cout << "------------------- Error ------------------------\n";

        std::expected<double, int> err = std::unexpected(3);
        basic_functions_tests(err);
    }

    void Test_Error()
    {
        std::expected<double, int> ex = std::unexpected(3);

        isError(ex);

        if (ex == std::unexpected(3))
            std::cout << "The error value is equal to 3\n";

        if (ex.error() == 3)
            std::cout << "The error value is equal to 3 (Same as above)\n";
    }


    void Test_OK()
    {
        std::expected<double, int> ex = 3.14;
        isError(ex);
    }
};

void Expected::TestAll()
{
    BasicFunctions();

    // Test_OK();
    // Test_Error();
};

