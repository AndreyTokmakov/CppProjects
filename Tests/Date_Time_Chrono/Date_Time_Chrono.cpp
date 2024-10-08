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
#include <format>
#include <thread>

namespace Date_Time_Chrono
{
    using namespace std::chrono;
    using namespace std::literals;
    using namespace std::chrono_literals;


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

    void Year_Month_Day_Test_2()
    {
        const std::chrono::year_month_day day1 = std::chrono::April/7/2018;
        std::cout << "'" << day1 << "' is " << std::format("{:%A}\n", std::chrono::weekday(day1));

        const std::chrono::year_month_day day2 = 2018y/April/8;
        std::cout << "'" << day2 << "' is " << std::format("{:%A}\n", std::chrono::weekday(day2));

        const std::chrono::year_month_day bad_day = January/0/2024;
        if (!bad_day.ok())
            std::cout << "'" << bad_day << "' is not a valid day\n";
    }
}

namespace Date_Time_Chrono::TimeToString
{
    constexpr char FORMAT[] { "%d-%02d-%02d %02d:%02d:%02d.%06ld" };

    std::string getCurrentTime(const time_point<system_clock>& timestamp = system_clock::now()) noexcept
    {
        const time_t time { std::chrono::system_clock::to_time_t(timestamp) };
        std::tm tm {};
        ::localtime_r(&time, &tm);

        std::string buffer(64, '\0');
        const int32_t size = std::sprintf(buffer.data(), FORMAT,
                     tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                     duration_cast<microseconds>(timestamp - time_point_cast<seconds>(timestamp)).count());
        buffer.resize(size);
        buffer.shrink_to_fit();
        return buffer;
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

namespace Date_Time_Chrono::Experiments
{
    void test()
    {
        [[maybe_unused]] std::chrono::gps_clock gpsClock;

        // std::cout << gpsClock.

    }
}


namespace Date_Time_Chrono::Sleep
{
    template<typename Predicate>
    void sleep_conditional(const uint32_t milliseconds,
                           Predicate predicate,
                           const uint32_t wakeupIntervalMs = 100)
    {
        uint32_t timeout = milliseconds / wakeupIntervalMs;
        while (timeout-- > 0 && predicate())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(wakeupIntervalMs));
        }
    }


    void Test()
    {
        std::cout << TimeToString::getCurrentTime() << std::endl;
        bool run = true;
        {
            std::jthread t1 ([&] {
                sleep_conditional(5 * 1000, [&] -> bool { return run; });
            });

            std::jthread t2 ([&] {
                std::this_thread::sleep_for(std::chrono::seconds(2U));
                run = false;
            });
        }
        std::cout << TimeToString::getCurrentTime() << std::endl;
    }
}

void Date_Time_Chrono::TestAll()
{
    // ChronoTests();
    // Year_Month_Day_Test();
    // Year_Month_Day_Test_2();
    // TimeToString::Test();
    // Experiments::test();


    Sleep::Test();

}
