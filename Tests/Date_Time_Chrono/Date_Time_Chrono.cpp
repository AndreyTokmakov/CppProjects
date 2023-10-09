/**============================================================================
Name        : Date_Time_Chrono.cpp
Created on  : 16.07.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Date_Time_Chrono
============================================================================**/

#include "Date_Time_Chrono.h"

#include <iostream>
#include <chrono>

namespace Date_Time_Chrono
{
    using namespace std::chrono;
    using namespace std::literals;

    std::ostream& operator<<(std::ostream& stream,
                             const std::chrono::year_month_day& ymd)
    {
        stream << static_cast<int>(ymd.year()) << " / "
               << static_cast<unsigned>(ymd.month()) << " / "
               << static_cast<unsigned>(ymd.day()) ;

        return stream;
    }

    void ChronoTests()
    {
        using namespace std::chrono;
        using namespace std::chrono_literals;

        std::chrono::year_month_day startDay = std::chrono::day {1} / 2 / 2023;
        std::cout << startDay << std::endl;


        std::chrono::year_month_day d2 { year {2023}, month {3}, day{14}};
        std::cout << d2 << std::endl;

        /*
        for (auto d = startDay; d.month() == startDay.month(); d += std::chrono::months{1}) {
            std::cout << d << '\n';
        }*/
    }

    void Year_Month_Day_Test()
    {
        std::cout << "USA switching to summer time on "
                  << year_month_day{2023y/March/Sunday[2]} << "\n";
        std::cout << "Europe switching to summer time on "
                  << year_month_day{2023y/March/Sunday[last]} << "\n\n";

    }
}

namespace Date_Time_Chrono::TimeToString
{
    std::string getCurrentTime() noexcept {
        const std::chrono::time_point now { std::chrono::system_clock::now() };
        const time_t in_time_t { std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) };
        const std::chrono::duration nowMs = std::chrono::duration_cast<std::chrono::microseconds>(
                now.time_since_epoch()) % 1000000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%a %b %d %Y %T")
           << '.' << std::setfill('0') << std::setw(6) << nowMs.count();
        return ss.str();
    }

    std::string getDaytimeString()
    {
        time_t now = time(nullptr);
        std::string timeStr {ctime(&now)};
        timeStr.pop_back();
        return timeStr;
    }

    std::string formatToString()
    {
        std::string buffer;
        buffer.reserve(32);
        std::format_to(std::back_inserter(buffer), "{:%Y-%m-%d %H:%M:%OS}", std::chrono::system_clock::now());
        return buffer;
    }

    void Test()
    {
        std::cout << std::quoted(getCurrentTime()) << std::endl;
        std::cout << std::quoted(getDaytimeString()) << std::endl;
        std::cout << std::quoted(formatToString()) << std::endl;
    }
}

void Date_Time_Chrono::TestAll()
{
    // ChronoTests();
    // Year_Month_Day_Test();

    TimeToString::Test();
}
