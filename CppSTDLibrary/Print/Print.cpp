/**============================================================================
Name        : Print.cpp
Created on  : 19.08.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Print.cpp
============================================================================**/

#include "Print.h"

#include <print>
#include <chrono>


namespace Print
{
    void Print_Bits()
    {
        std::println("{:b}", std::bit_cast<uint64_t>(1.0));
    }
}

namespace Print::Chrono_Data_Time
{
    /**
     * Advanced Formatting Options
     * Year and Month:
     *      {0:%Y}: Full year (e.g., 2024).
     *      {0:%y}: Last two digits of the year (e.g., 24).
     *      {0:%B}: Full month name (e.g., November).
     *      {0:%b}: Abbreviated month name (e.g., Nov).
     *
     * Day and Week:
     *      {0:%d}: Day of the month, zero-padded (e.g., 01).
     *      {0:%A}: Full weekday name (e.g., Friday).
     *      {0:%a}: Abbreviated weekday name (e.g., Fri).
     *
     * Time of Day:
     *      {0:%H}: Hour (24-hour clock), zero-padded (e.g., 14).
     *      {0:%I}: Hour (12-hour clock), zero-padded (e.g., 02).
     *      {0:%M}: Minute, zero-padded (e.g., 05).
     *      {0:%S}: Second, zero-padded (e.g., 09).
     *      {0:%p}: AM/PM designation.
     *
     * Time Zone:
     *      {0:%Z}: Time zone abbreviation (e.g., UTC).
     *      {0:%z}: Offset from UTC (e.g., +0000).
     *
    **/
    void Print_Formating()
    {
        auto now = std::chrono::system_clock::now();
        std::print("now is {}", now);
        std::print("Full date and time: {0:%Y-%m-%d %H:%M:%S}\n", now);
        std::print("Date only: {0:%F}\n", now);
        std::print("Time only: {0:%T}\n", now);
        std::print("Day of the week: {0:%A}\n", now);
        std::print("Month name: {0:%B}\n", now);
        std::print("12-hour clock with AM/PM: {0:%I:%M:%S %p}\n", now);
        std::print("ISO 8601 format: {0:%FT%T%z}\n", now);
    }

}

void Print::TestAll()
{
    Print_Bits();

    Chrono_Data_Time::Print_Formating();

    /*
    constexpr std::string_view name = "Daniel"sv;
    std::println("Hello, {0} today is {1:%Y-%m-%d %X}, good day {0}!", name, system_clock::now());
    // Hello, Daniel today is 2024-11-02 00:02:17, good day Daniel!
    */
}