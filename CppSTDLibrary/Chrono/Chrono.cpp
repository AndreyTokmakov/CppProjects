//============================================================================
// Name        : Chrono.cpp
// Created on  : 31.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Chrono src class
//============================================================================

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>
#include "Chrono.h"
#include <time.h>
#include <stdio.h>

namespace Chrono {

    void Duration_TimePoint_Print()
    {
        std::chrono::duration<int, std::ratio<60 * 60 * 24> > one_day(1);

        std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point tomorrow = today + one_day;

        time_t time;

        time = std::chrono::system_clock::to_time_t(today);

        /*
        char str[26];
        std::ctime_s(str, sizeof str, &time);
        std::cout << "today is: " << str << std::endl;

        time = std::chrono::system_clock::to_time_t(tomorrow);
        ctime_s(str, sizeof str, &time);
        std::cout << "tomorrow will be: " << str << std::endl;
        */
    }


    void Measure_Duration() {
        const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        const int TIMEOUT = 10;

        for (int i = 0; i < 20; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            std::chrono::duration<int> timeElapsed = std::chrono::duration_cast<std::chrono::duration<int>>(end - start);
            std::cout << "Time passed: " << timeElapsed.count() << std::endl;
            if (timeElapsed.count() >= TIMEOUT)
            {
                std::cout << "TIMEOUT";
                break;
            }
        }
    }

    void Steady_clock()
    {
        using namespace std::chrono;
        steady_clock::time_point t1 = steady_clock::now();

        std::cout << "printing out 1000 stars..." << std::endl;
        for (int i = 0; i < 1000; ++i)
            std::cout << "*";
        std::cout << std::endl;

        steady_clock::time_point t2 = steady_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
        std::cout << "It took me " << time_span.count() << " seconds." << std::endl;
    }

    void Clock_Test() {
        std::chrono::system_clock::time_point time_point_now = std::chrono::system_clock::now();
        time_t time = std::chrono::system_clock::to_time_t(time_point_now);


        /*
        char str[26];
        ctime_s(str, sizeof str, &time);
        std::cout << "today is: " << str << std::endl;
        */
    }

    void Asctime()
    {
        std::time_t result = std::time(nullptr);
        std::cout << std::asctime(std::localtime(&result)) << result << " seconds since the Epoch" << std::endl;
    }

    void GM_time_VS_localtime()
    {
        std::time_t t = std::time(nullptr);
        std::cout << "UTC:       " << std::put_time(std::gmtime(&t), "%c %Z") << '\n';
        std::cout << "local:     " << std::put_time(std::localtime(&t), "%c %Z") << '\n';


        // POSIX-specific:
        /*
        std::string tz = "TZ=Asia/Singapore";
        putenv(tz.data());
        std::cout << "Singapore: " << std::put_time(std::localtime(&t), "%c %Z") << '\n';
        */
    }

    void High_Resolution_Clock()
    {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        std::cout << "printing out 1000 stars...\n";
        for (int i = 0; i < 1000; ++i)
            std::cout << "*";
        std::cout << std::endl;

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(start - end);

        std::cout << "It took me " << time_span.count() << " seconds.\n";
    }

    void PrintLocaltime()
    {
        auto now = std::chrono::system_clock::now();
        auto stime = std::chrono::system_clock::to_time_t(now);
        auto ltime = std::localtime(&stime);
        std::cout << std::put_time(ltime, "%c") << std::endl;
    }

    void PrintCurrenTime1()
    {
        std::time_t t = std::time(nullptr);
        std::cout << std::put_time(std::gmtime(&t), "%c %Z") << '\n';
        std::cout << std::put_time(std::gmtime(&t), "%c") << '\n';

        std::cout << std::endl;

        std::cout << std::put_time(std::localtime(&t), "%c %Z") << '\n';
    }

    void PrintCurrenTime2() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::cout << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << std::endl;
    }

    void GetCurrentTime_Performance() {
        constexpr int COUNT {100'000'000};

        {
            auto start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < COUNT;  ++i) {
                std::time_t t = std::time(nullptr);
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "Result: " << duration << " microseconds" << std::endl;
        }

        {
            auto start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < COUNT; ++i) {
                auto now = std::chrono::system_clock::now();
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "Result: " << duration << " microseconds" << std::endl;
        }
    }

    void Time_T()
    {
        std::time_t result = std::time(nullptr);
        std::cout << std::ctime(&result);
    }

    void Localtime_TM()
    {
        time_t rawtime;
        tm* timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("Current local time and date: %s", asctime(timeinfo));

        std::cout << "Hour: " << timeinfo->tm_hour << std::endl;
        std::cout << "Min: " << timeinfo->tm_min << std::endl;
        std::cout << "Sec: " << timeinfo->tm_sec << std::endl;
    }

    void Zones_Tests()
    {
        time_t rawtime;
        tm* ptm;

        static constexpr int MST = -7;
        static constexpr int UTC = 0;
        static constexpr int CCT = 8;

        time(&rawtime);

        ptm = gmtime(&rawtime);

        puts("Current time around the World:");
        printf("Phoenix, AZ (U.S.) :  %2d:%02d\n", (ptm->tm_hour + MST) % 24, ptm->tm_min);
        printf("Reykjavik (Iceland) : %2d:%02d\n", (ptm->tm_hour + UTC) % 24, ptm->tm_min);
        printf("Beijing (China) :     %2d:%02d\n", (ptm->tm_hour + CCT) % 24, ptm->tm_min);
    }

    void TIme_Format() {

        //auto now = std::chrono::system_clock::now();
        //std::cout << date::format("%T", std::chrono::floor<std::chrono::milliseconds>(now));

        //auto now = std::chrono::system_clock::now();
        //auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch() % std::chrono::seconds{ 1 });
        //std::cout << std::date::format("%X", std::chrono::floor<std::chrono::milliseconds>(now)) << "," << ms.count();
    }

    void Year_Month_Day() {
        const std::chrono::time_point now { std::chrono::system_clock::now() };
        const std::chrono::year_month_day ymd {std::chrono::floor<std::chrono::days>(now) };

        std::cout << "Current Year: " << static_cast<int>(ymd.year())
                  << ", Month: " << static_cast<unsigned>(ymd.month())
                  << ", Day: " << static_cast<unsigned>(ymd.day()) << '\n';
    }

    void Create_Day_Manually()
    {
        using namespace std::chrono_literals;
        constexpr std::chrono::year_month_day d = std::chrono::day{31}/1/2021;

        std::cout << "Current Year: " << static_cast<int>(d.year())
                  << ", Month: " << static_cast<unsigned>(d.month())
                  << ", Day: " << static_cast<unsigned>(d.day()) << '\n';

    }

    void Enumerate_Each_Month_of_Year()
    {
        using namespace std::chrono_literals;

        std::chrono::year_month_day first = 2021y / 1 / 5;
        for (auto d = first; d.year() == first.year(); d += std::chrono::months{1}) {
            std::cout << static_cast<int>(d.year()) << " / "
                      << static_cast<unsigned>(d.month()) << " / "
                      << static_cast<unsigned>(d.day()) << '\n';
        }
    }


    void is_PM_AM() {
        using namespace std::chrono_literals;

        static_assert(
                is_am(10h) &&  is_am(11h) && !is_am(12h) && !is_am(23h) &&
                !is_pm(10h) && !is_pm(11h) &&  is_pm(12h) &&  is_pm(23h)
        );
    }

    void H24_to_AM_PM() {
        using namespace std::chrono_literals;

        for (const std::chrono::hours  hh : { 0h, 1h, 11h, 12h, 13h, 23h }) {
            const std::chrono::hours  am{make12(hh)};
            std::cout << std::setw(2) << hh.count() << "h == "
                      << std::setw(2) << am.count() << (is_am(hh) ? "h a.m.\n" : "h p.m.\n");
        }

        std::cout << "\nmake24():\n";

        using p = std::pair<std::chrono::hours , bool>;

        for (const auto& [hh, pm] : { p{1h, 0}, p{12h, 0}, p{1h, 1}, p{12h, 1} }) {
            std::cout << std::setw(2) << hh.count()
                      << (pm ? "h p.m." : "h a.m.")
                      << " == " << std::setw(2)
                      << make24(hh, pm).count() << "h\n";
        }
    }

    void Experiments()
    {
        using namespace std::chrono_literals;

        std::chrono::year_month_day first = 2021y / 1 / 5;
        for (auto d = first; d.year() == first.year(); d += std::chrono::months{1}) {
            std::cout << static_cast<int>(d.year()) << " / "
                      << static_cast<unsigned>(d.month()) << " / "
                      << static_cast<unsigned>(d.day()) << '\n';

            //auto tp{std::chrono::sys_days{d} + 18h + 30min};
            //std::cout << " UTC:" << tp << ' \n ' ;
        }


    }
};

namespace Chrono::Duration {

    void HighResolution__PeriodDuration() {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        std::this_thread::sleep_for(std::chrono::milliseconds(125));

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

        std::cout << "It took me " << time_span.count() << " seconds.\n";
    }

    void SteadyClock__PeriodDuration() {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        std::this_thread::sleep_for(std::chrono::milliseconds(125));

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "It took me " << time_span.count() << " seconds." << std::endl;
    }

    void Zero_Duration()
    {
        using std::chrono::steady_clock;
        steady_clock::time_point start = steady_clock::now();

        // std::this_thread::sleep_for(std::chrono::milliseconds(125));

        steady_clock::time_point end = steady_clock::now();
        steady_clock::duration elapsedTime = end - start;

        if (elapsedTime == steady_clock::duration::zero())
            std::cout << "The internal clock did not tick.\n";
        else
            std::cout << "The internal clock advanced " << elapsedTime.count() << " periods.\n";
    }

    void Min_Max()
    {
        std::cout << "system_clock durations can represent:\n";
        std::cout << "min: " << std::chrono::system_clock::duration::min().count() << "\n";
        std::cout << "max: " << std::chrono::system_clock::duration::max().count() << "\n";
    }

    void DurationCast()
    {
        std::chrono::seconds one_second(1);
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(one_second);

        ms += std::chrono::milliseconds(2500);  // 2500 millisecond
        one_second = std::chrono::duration_cast<std::chrono::seconds> (ms);

        std::cout << "milliseconds: " << ms.count() << std::endl;
        std::cout << "seconds     : " << one_second.count() << std::endl;
    }
};

namespace Chrono::TimeZones {

    using namespace std::chrono;

    void Test() {
        // auto x = std::chrono::time_zone::name();

        // constexpr auto ym { year(2021) / 8 };
        // std::cout << (ym == year_month(year(2021), August)) << ' ';
    }
}

namespace Chrono::StringFormat
{
    std::string FormatGmt(const std::tm& tm)
    {
        std::array<char, 100> buff {};
        std::strftime(buff.data(), buff.size(), "%A %c", &tm);
        return std::string{buff.data()};
    }

    void StrfTime() {
        const std::time_t t = std::time(nullptr);
        const tm* timeInfo = std::localtime(&t);

        std::cout << FormatGmt(*timeInfo) << std::endl;
    }

    void Asctime()
    {
        const std::time_t t = std::time(nullptr);
        const tm* timeInfo = std::localtime(&t);

        std::string_view strTime(asctime(timeInfo));
        strTime.remove_suffix(1);

        std::cout << strTime << std::endl;
    }

    void PutTime_To_String()
    {
        const std::time_t t = std::time(nullptr);
        const tm* timeInfo = std::localtime(&t);

        std::ostringstream oss {};
        oss << std::put_time(timeInfo, "%d-%m-%Y %H-%M-%S");
        auto str = oss.str();

        std::cout << str << std::endl;
    }

    void CTime_String() {
        const std::time_t ct { std::time(0) };
        const std::string_view timeStr { ctime(&ct) };

        std::cout << timeStr << std::endl;
    }

    void Format() {
        const auto now = std::chrono::system_clock::now();
        // std::cout << std::format("{:%d-%m-%Y %H:%M:%OS}", now) << '\n';
    }
}

uint64_t format_date(std::string_view str)
{
    int h, m, s, ms;
    sscanf(str.data(), "%d:%d:%d.%d", &h, &m, &s, &ms);
    return h * m * s * ms;
}


void Chrono::TestAll()
{
    // Steady_clock();
    // Duration_TimePoint_Print();
    // Clock_Test();

    // Measure_Duration();

    // High_Resolution_Clock();

    // Asctime();
    // GM_time_VS_localtime();
    // Time_T();

    // Zones_Tests();

    // Localtime_TM();

    // PrintLocaltime();

    // PrintCurrenTime1();
    // PrintCurrenTime2();
    // GetCurrentTime_Performance();

    // Duration::HighResolution__PeriodDuration();
    // Duration::SteadyClock__PeriodDuration();
    // Duration::Zero_Duration();
    // Duration::Min_Max();
    // Duration::DurationCast();

    // TimeZones::Test();

    StringFormat::StrfTime();
    // StringFormat::Asctime();
    // StringFormat::PutTime_To_String();
    // StringFormat::CTime_String();
    // StringFormat::Format();

    // Year_Month_Day();
    // Create_Day_Manually();
    // Enumerate_Each_Month_of_Year();

    // is_PM_AM();
    // H24_to_AM_PM();

    // Experiments();

    // auto v =format_date("11:22:33.123123");
    // std::cout << v << std::endl;

};

