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
#include <vector>

#include "Expected.h"

namespace Expected
{
    enum class ParseError {
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


    void Test_Error_2()
    {
        std::expected<double, ParseError> ex = std::unexpected(ParseError::InvalidChar);

        if (!ex) {
            if (ex == std::unexpected(ParseError::InvalidChar)) {
                std::cout << "InvalidChar\n";
            } else if (ex == std::unexpected(ParseError::InvalidChar)) {
                std::cout << "Overflow\n";
            }
        }
    }


    void Test_OK()
    {
        std::expected<double, int> ex = 3.14;
        isError(ex);
    }

    void Emplace()
    {
        std::expected<double, int> ex;
        basic_functions_tests(ex);

        std::cout << "------------------- After emplace: 1 ------------------------\n";

        ex.emplace(1);
        basic_functions_tests(ex);
    }
};

namespace Expected
{
    std::expected<int, std::string> getInt(const std::string& arg)
    {
        try {
            return std::stoi(arg);
        }
        catch (...) {
            return std::unexpected{std::string(arg + ": Error")};
        }
    }

    void Transform()
    {
        const std::vector<std::string> strings = {"66", "foo", "-5"};
        for (const std::string& str: strings)
        {
            std::expected res = getInt(str)
                    .transform( [](int n) { return n + 100; })
                    .transform( [](int n) { return std::to_string(n); });

            std::cout << *res << std::endl;
        }
    }
}

namespace Expected
{
    std::expected<std::string, std::error_condition> read_input() {
        std::string s;
        if (not (std::cin >> s))
            return std::unexpected{std::make_error_condition(std::io_errc::stream)};
        return s;
    }

    std::expected<int, std::error_condition> to_int(const std::string& s) {
        try {
            return std::stoi(s);
        } catch (std::exception& e) {
            return std::unexpected{std::make_error_condition(std::errc::argument_out_of_domain)};
        }
    }

    int add_ten(int v) { return v + 10; }

    std::expected<int, std::error_condition> log_error(const std::error_condition& err) {
        std::cerr << "Operation failed : " << err.message() << "\n";
        return std::unexpected{err};
    }

    void transform__and_then()
    {
        std::expected result = read_input()
                .and_then(to_int)    // invoked if the expected contains a value  the callable has to return a std::expected,
                                        // but can change the type std::expected<T,Err> -> std::expected<U,Err>
                .transform(add_ten)  // invoked if the expected contains a value
                                        // the callable can return any type std::expected<T,Err> -> std::expected<U,Err>
                .or_else(log_error); // invoked if the expected contains an error the callable has to return a std::expected,
                                        // but can change the type std::expected<V,T> -> std::expected<V,U>

        std::cout << *result << std::endl;
    }
}

void Expected::TestAll()
{
    // BasicFunctions();

    // Test_OK();

    // Test_Error();
    // Test_Error_2();

    // Emplace();

    // Expected::Transform();

    // transform__and_then();
};

