/**============================================================================
Name        : Number_Parser.cpp
Created on  : 31.12.2025
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "StringViewTests.h"

#include <charconv>
#include <iostream>
#include <cctype>
#include <optional>
#include <type_traits>
#include <iomanip>

namespace StringView::number_parser::utils
{
    constexpr std::string_view whitespace { " \t\n\r\f\v" };
    constexpr std::string_view trueStr { "true" };
    constexpr std::string_view falseKey { "false" };
    constexpr std::string_view trueNum { "1" };
    constexpr std::string_view falseNum { "0" };

    [[nodiscard]]
    constexpr std::string_view trim(const std::string_view str) noexcept
    {
        const size_t start{str.find_first_not_of(whitespace)};
        if (std::string_view::npos == start) {
            return {};
        }

        const size_t end { str.find_last_not_of(whitespace) };
        return str.substr(start, end - start + 1);
    }

    [[nodiscard]]
    constexpr bool isEqual(std::string_view l_str, std::string_view r_str) noexcept
    {
        if (l_str.size() != r_str.size()) {
            return false;
        }

        return std::ranges::equal(l_str, r_str, [](const char l_chr, const char r_chr) {
            return std::tolower(static_cast<unsigned char>(l_chr)) ==
                std::tolower(static_cast<unsigned char>(r_chr));
        });
    }

    template<typename T>
    [[nodiscard]] std::optional<T> parseNumber(std::string_view str) noexcept
    {
        T value{};
        const auto [ptr, ec] { std::from_chars(str.data(), str.data() + str.size(), value) };
        if (std::errc{} == ec && ptr == str.data() + str.size()) {
            return value;
        }

        return std::nullopt;
    }
    template<typename T>
    concept Parsable = std::is_arithmetic_v<T> || std::is_enum_v<T>;

    template<Parsable T>
    [[nodiscard]]
    std::optional<T> fromString(const std::string_view input) noexcept
    {
        const std::string_view trimmed_str { trim(input) };
        if (trimmed_str.empty()) {
            return std::nullopt;
        }

        if constexpr (std::is_same_v<T, bool>)
        {
            if (trueNum == trimmed_str || isEqual(trimmed_str, trueStr)) {
                return true;
            }
            if (falseNum == trimmed_str || isEqual(trimmed_str, falseKey)) {
                return false;
            }
            return std::nullopt;
        }
        else if constexpr (std::is_enum_v<T>)
        {
            using undertype_t = std::underlying_type_t<T>;
            if (const auto val { parseNumber<undertype_t>(trimmed_str) }) {
                return static_cast<T>(*val);
            }
            return std::nullopt;
        }
        else
        {
            return parseNumber<T>(trimmed_str);
        }
    }
}

namespace StringView::number_parser::tests
{
    enum class Status : uint8_t
    {
        IDLE = 0,
        RUNNING = 1,
        ERROR = 255
    };

    template <typename T>
    void run_test(const std::string_view input,
                  std::optional<T> expected,
                  const std::string_view testName)
    {
        std::optional<T> result { utils::fromString<T>(input) };
        const bool pass { (result == expected) };

        std::cout << "[" << (pass ? "PASS" : "FAIL") << "] "
                  << std::left << std::setw(30) << testName
                  << " Input: '" << input << "'";

        if (!pass)
        {
            std::cout << " (Expected: ";
            if(expected) std::cout << "Value"; else std::cout << "nullopt";
            std::cout << ", Got: ";
            if(result) std::cout << "Value"; else std::cout << "nullopt";
            std::cout << ")";
        }
        std::cout << "\n";
    }
}



void StringView::number_parser::TestAll()
{
    using namespace StringView::number_parser::tests;

    std::cout << "--- Integer Tests ---\n";
    run_test<int>("123", 123, "Simple Int");
    run_test<int>("  -42  ", -42, "Trimmed Negative");
    run_test<int>("12abc", std::nullopt, "Partial Match Fail");
    run_test<int>("", std::nullopt, "Empty String");


    std::cout << "\n--- Overflow Tests ---\n";
    run_test<uint8_t>("255", 255, "Uint8 Max");
    run_test<uint8_t>("256", std::nullopt, "Uint8 Overflow");

    std::cout << "\n--- Float Tests ---\n";
    run_test<double>("3.14159", 3.14159, "Simple Double");
    run_test<float>("  1.5e2 ", 150.0f, "Scientific Notation");
    run_test<float>("invalid", std::nullopt, "Invalid Float");

    std::cout << "\n--- Bool Tests ---\n";
    run_test<bool>("true", true, "Lower true");
    run_test<bool>("TRUE", true, "Upper TRUE");
    run_test<bool>("  True  ", true, "Mixed Trimmed");
    run_test<bool>("1", true, "Numeric 1");
    run_test<bool>("false", false, "Lower false");
    run_test<bool>("0", false, "Numeric 0");
    run_test<bool>("yes", std::nullopt, "Invalid 'yes'");
    run_test<bool>("2", std::nullopt, "Invalid number '2'");

    std::cout << "\n--- Enum Tests ---\n";
    run_test<Status>("0", Status::IDLE, "Enum 0 (Idle)");
    run_test<Status>("  1 ", Status::RUNNING, "Enum 1 (Running)");
    run_test<Status>("255", Status::ERROR, "Enum 255 (Error)");
    run_test<Status>("3", std::optional<Status>{static_cast<Status>(3)}, "Enum Unknown Value");
}


/**
--- Integer Tests ---
[PASS] Simple Int                     Input: '123'
[PASS] Trimmed Negative               Input: '  -42  '
[PASS] Partial Match Fail             Input: '12abc'
[PASS] Empty String                   Input: ''

--- Overflow Tests ---
[PASS] Uint8 Max                      Input: '255'
[PASS] Uint8 Overflow                 Input: '256'

--- Float Tests ---
[PASS] Simple Double                  Input: '3.14159'
[PASS] Scientific Notation            Input: '  1.5e2 '
[PASS] Invalid Float                  Input: 'invalid'

--- Bool Tests ---
[PASS] Lower true                     Input: 'true'
[PASS] Upper TRUE                     Input: 'TRUE'
[PASS] Mixed Trimmed                  Input: '  True  '
[PASS] Numeric 1                      Input: '1'
[PASS] Lower false                    Input: 'false'
[PASS] Numeric 0                      Input: '0'
[PASS] Invalid 'yes'                  Input: 'yes'
[PASS] Invalid number '2'             Input: '2'

--- Enum Tests ---
[PASS] Enum 0 (Idle)                  Input: '0'
[PASS] Enum 1 (Running)               Input: '  1 '
[PASS] Enum 255 (Error)               Input: '255'
[PASS] Enum Unknown Value             Input: '3'
**/