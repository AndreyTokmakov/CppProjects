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

void Date_Time_Chrono::TestAll()
{
    ChronoTests();
    Year_Month_Day_Test();
}
