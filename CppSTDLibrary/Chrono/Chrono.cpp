//============================================================================
// Name        : Chrono.cpp
// Created on  : 31.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Chrono src class
//============================================================================

#define _CRT_SECURE_NO_WARNINGS

#include "Chrono.h"
#include "../Helpers/ScopedTimer.h"

#include <iostream>
#include <syncstream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>
#include <cstdio>


namespace Chrono
{
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


    void TIme_Format() {

        //auto now = std::chrono::system_clock::now();
        //std::cout << date::format("%T", std::chrono::floor<std::chrono::milliseconds>(now));

        //auto now = std::chrono::system_clock::now();
        //auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch() % std::chrono::seconds{ 1 });
        //std::cout << std::date::format("%X", std::chrono::floor<std::chrono::milliseconds>(now)) << "," << ms.count();
    }

    void Year_Month_Day()
    {
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

            auto tp{std::chrono::sys_days{d} + 18h + 30min};
            //std::cout << " UTC:" << tp << ' \n ' ;
        }


    }
};

namespace Chrono::Duration
{
    void Create_Simple()
    {
        const std::chrono::seconds seconds10 {10};
        const std::chrono::minutes minutes5 {5};

        std::cout << (minutes5 > seconds10 ? "minutes5  > seconds1" : "seconds10 > minutes5") << std::endl;

        const std::chrono::seconds seconds20 { seconds10 + seconds10 };
        std::cout << (seconds20 > seconds10 ? "seconds20 > seconds10" : "seconds10 > seconds20") << std::endl;

        const std::chrono::minutes minutes10 { minutes5 * 2 };
        std::cout << (minutes10 > minutes5 ? "minutes10 > minutes5" : "minutes5 > minutes10") << std::endl;
    }

    void Measure_Duration()
    {
        const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        constexpr int32_t timeout { 10 };

        for (int i = 0; i < 20; ++i)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                std::chrono::duration<int> timeElapsed = std::chrono::duration_cast<std::chrono::duration<int>>(end - start);
            std::cout << "Time passed: " << timeElapsed.count() << std::endl;

            if (timeElapsed.count() >= timeout)
            {
                std::cout << "TIMEOUT";
                break;
            }
        }
    }

    void HighResolution__PeriodDuration()
    {
        const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        std::this_thread::sleep_for(std::chrono::milliseconds(125));

        const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

        std::cout << "It took me " << time_span.count() << " seconds.\n";
    }

    void SteadyClock__PeriodDuration()
    {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        std::this_thread::sleep_for(std::chrono::microseconds (125));

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "It took me " << time_span.count() << " seconds." << std::endl;
    }

    void Time_From_Duration()
    {
        const std::chrono::duration<int, std::ratio<60 * 60 * 24> > one_day(1);
        const std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
        const std::chrono::system_clock::time_point tomorrow = today + one_day;

        {
            time_t time = std::chrono::system_clock::to_time_t(today);
            tm* localTime = std::localtime(&time);
            std::cout << "Today: " << std::put_time(localTime, "%c") << std::endl;
        }
        {
            time_t time = std::chrono::system_clock::to_time_t(tomorrow);
            tm* localTime = std::localtime(&time);
            std::cout << "Today: " << std::put_time(localTime, "%c") << std::endl;
        }
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


    void Measure_Duration_Test()
    {
        using namespace std::literals;

        const std::chrono::time_point<std::chrono::steady_clock> tp1 = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(1ms);
        const std::chrono::time_point<std::chrono::steady_clock> tp2 = std::chrono::steady_clock::now();

        {
            // Whether a clock is steady and its resolution can be queried using:
            bool system_is_steady = std::chrono::system_clock::is_steady;
            std::cout << std::format("system_is_steady == {}", system_is_steady) << std::endl;

            using resolution = std::chrono::system_clock::duration;
            /** resolution::period::num / resolution::period::den **/
            std::cout << std::format("resolution == {}/{}", resolution::period::num, resolution::period::den) << std::endl;
        }

        std::cout << std::endl;

        {
            /** difference of time points is a duration **/
            const std::chrono::duration duration = tp2 - tp1;
            std::cout << std::format("duration (nanoseconds)  : {}\n", duration);

            /** explicit type of duration, base type double, with micro resolution **/
            const std::chrono::duration<double, std::micro> fpdur = tp2 - tp1;
            // std::cout << std::format("fpdur == {}\n", fpdur) << std::endl;

            const std::chrono::duration duration_millis = duration_cast<std::chrono::milliseconds>(tp2 - tp1);
            std::cout << std::format("duration (milliseconds) : {}\n", duration_millis);
        }

        std::cout << std::endl;

        {
            using day_t = std::chrono::duration<double, std::ratio<86400>>;
            // days, weeks, months, years were added in C++20

            // A year duration is the length of an average year
            day_t days_in_year = std::chrono::years{1};
            // days_in_year == 365.2425

            std::cout << std::format("Days in Year  : {:%Q%q}", days_in_year) << std::endl;

            // A month is 1/12 of a year
            day_t days_in_month = std::chrono::months{1};
            // days_in_month == days_in_year / 12 == 30.436875

            std::cout << std::format("Days in Month : {:%Q%q}", days_in_month) << std::endl;
        }

    }
};

namespace Chrono::StringFormat
{
    std::string FormatGmt(const std::tm& tm)
    {
        std::array<char, 100> buff {};
        std::strftime(buff.data(), buff.size(), "%A %c", &tm);
        return std::string{buff.data()};
    }

    void StrfTime()
    {
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

    void CTime_String()
    {
        const std::time_t ct { std::time(0) };
        const std::string_view timeStr { ctime(&ct) };

        std::cout << timeStr << std::endl;
    }

    struct CurrentTime
    {
        const std::chrono::time_point<std::chrono::system_clock> now { std::chrono::system_clock::now() };
    };

    struct SyncTimeStream
    {
        const std::chrono::time_point<std::chrono::high_resolution_clock> now { std::chrono::system_clock::now() };

        template<class T>
        std::osyncstream operator<<(T&& s)
        {
            std::osyncstream stream {std::cout} ;
            stream << std::format("{:%d-%m-%Y %H:%M:%OS}", now) << std::forward<T>(s);
            return stream;
        }
    };

    std::ostream& operator<<(std::ostream& stream, const CurrentTime& time)
    {
        stream << std::format("{:%d-%m-%Y %H:%M:%OS}", time.now);
        return stream;
    }



    void Format()
    {
        const std::chrono::time_point now = std::chrono::system_clock::now();
        std::cout << std::format("{:%d-%m-%Y %H:%M:%OS}", now) << '\n';
    }

    void Format2()
    {
        std::cout << CurrentTime{} << '\n';
        std::osyncstream {std::cout} << CurrentTime{} << '\n';
    }

    void Format3()
    {
        SyncTimeStream{} << 2 << "  sds " << 1;
    }


    void StringToTime()
    {
        constexpr std::string_view timeStr {"Tue 30/10/2001 10:59:10 AM"};
        tm dateTime {};
        strptime(timeStr.data(), "%a %d/%m/%Y %r", &dateTime);

        std::cout << dateTime.tm_mday << "/" << dateTime.tm_mon + 1 << "/"<< dateTime.tm_year + 1900 << ' '
                  << dateTime.tm_hour << ':' << dateTime.tm_min << ':' << dateTime.tm_sec
                  << std::endl;
    }
}

uint64_t format_date(std::string_view str)
{
    int h, m, s, ms;
    sscanf(str.data(), "%d:%d:%d.%d", &h, &m, &s, &ms);
    return h * m * s * ms;
}

namespace Chrono::TimeToString
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
        buffer.shrink_to_fit();
        return buffer;
    }

    void Test()
    {
        std::cout << std::quoted(getCurrentTime()) << std::endl;
        std::cout << std::quoted(getDaytimeString()) << std::endl;
        std::cout << std::quoted(formatToString()) << std::endl;
    }
}


namespace Chrono::TimeOfDay
{
    void TimeOfDay_Basics()
    {
        using namespace std::chrono_literals;
        std::cout << std::boolalpha << '\n';


        const std::chrono::hh_mm_ss<std::chrono::duration<long double>> timeOfDay {
            std::chrono::hh_mm_ss(10.5h + 98min + 2020s + 0.5s)
        };

        std::cout<< "timeOfDay: " << timeOfDay << "\n\n";

        std::cout << "timeOfDay.hours(): " << timeOfDay.hours() << '\n';
        std::cout << "timeOfDay.minutes(): " << timeOfDay.minutes() << '\n';
        std::cout << "timeOfDay.seconds(): " << timeOfDay.seconds() << '\n';
        std::cout << "timeOfDay.subseconds(): " << timeOfDay.subseconds() << '\n';
        std::cout << "timeOfDay.to_duration(): " << timeOfDay.to_duration() << "\n\n";

        std::cout << "std::chrono::hh_mm_ss(45700.5s): "<< std::chrono::hh_mm_ss(45700.5s) << '\n';
        std::cout << "std::chrono::is_am(5h): " << std::chrono::is_am(5h) << '\n';
        std::cout << "std::chrono::is_am(15h): " << std::chrono::is_am(15h) << '\n';
        std::cout << "std::chrono::make12(5h): " << std::chrono::make12(5h) << '\n';
        std::cout << "std::chrono::make12(15h): " << std::chrono::make12(15h) << '\n';
    }
}


namespace Chrono::CalendarDate
{
    using std::chrono::Monday;
    using std::chrono::Saturday;
    using std::chrono::March;
    using std::chrono::June;
    using std::chrono::July;
    using std::chrono::days;
    using std::chrono::months;
    using std::chrono::years;
    using std::chrono::last;
    using namespace std::chrono_literals;

    void Basics()
    {
        std::cout << std::boolalpha;

        std::cout << "March: " << March << '\n';
        std::cout << "March + months(3): " << March + months(3) << '\n';
        std::cout << "March - months(25): " << March - months(25) << '\n';
        std::cout << "July - June: " <<  July - June << '\n';
        std::cout << "June < July: " << (June < July) << "\n\n";

        std::cout << "Saturday: " << Saturday << '\n';
        std::cout << "Saturday + days(3): " << Saturday + days(3) << '\n';
        std::cout << "Saturday - days(22): " << Saturday - days(22) << '\n';
        std::cout << "Saturday - Monday: " <<  Saturday - Monday << "\n\n";

        std::cout << "2021y/March: " << 2021y/March << '\n';
        std::cout << "2021y/March + years(3) - months(35): " << 2021y/March + years(3) - months(35) << '\n';
        std::cout << "2022y/July - 2021y/June: " << 2022y/July - 2021y/June << '\n';
        std::cout << "2021y/June > 2021y/July: " << (2021y/June > 2021y/July) << "\n\n";


        std::cout << "2021y/March/Saturday[last]: " << 2021y/March/Saturday[last] << '\n';
        std::cout << "2021y/March/Saturday[last] + months(13) + years(3): "
                  << 2021y/March/Saturday[last] + months(13) + years(3) << '\n';
        std::cout << "2021y/July/Saturday[last] - months(1) == 2021y/June/Saturday[last]: "
                  << (2021y/July/Saturday[last] - months(1) == 2021y/June/Saturday[last])
                << "\n\n";
    }
}

namespace Chrono::FunctionPerformance
{
    void GetCurrentTime_Performance()
    {
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


    void TestGetCurrentTimeFunctions()
    {
        constexpr int32_t iterCount {100'000'000};

        {
            Helpers::ScopedTimer timer{"system_clock::now()"};
            for (int i = 0; i < iterCount; ++i)
            {
                std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
                //time_t time = std::chrono::system_clock::to_time_t(today);
            }
        }

        {
            Helpers::ScopedTimer timer{"high_resolution_clock::now()"};
            for (int i = 0; i < iterCount; ++i)
            {
                auto start = std::chrono::high_resolution_clock::now();
            }
        }

        {
            Helpers::ScopedTimer timer{"clock_gettime()"};
            timespec time{};
            for (int i = 0; i < iterCount; ++i)
            {
                clock_gettime(CLOCK_MONOTONIC, &time);
            }
        }

        {
            Helpers::ScopedTimer timer{"std::time(1)"};
            for (int i = 0; i < iterCount; ++i)
            {
                std::time_t t = std::time(nullptr);
                // std::cout << "UTC:       " << std::put_time(std::gmtime(&t), "%c %Z") << '\n';
            }
        }

        {
            Helpers::ScopedTimer timer{"std::time(2)"};
            time_t rawTime;
            for (int i = 0; i < iterCount; ++i)
            {
                std::time ( &rawTime );
            }
        }

        {
            Helpers::ScopedTimer timer{"std::timespec_get"};
            std::timespec ts {};
            for (int i = 0; i < iterCount; ++i)
            {
                std::timespec_get(&ts, TIME_UTC);
            }
        }
    }
}

namespace Chrono::Months
{
    using namespace std::chrono;
    using namespace std::chrono_literals;

    void Enumerate_Each_Month_of_Year()
    {
        const std::chrono::year_month_day first = 2021y / 1 / 5;
        for (std::chrono::year_month_day d = first; d.year() == first.year(); d += std::chrono::months{1}) {
            std::cout << static_cast<int>(d.year()) << " / "
                      << static_cast<unsigned>(d.month()) << " / "
                      << static_cast<unsigned>(d.day()) << '\n';
        }
    }

    void Month_of_Year()
    {
        for (std::chrono::year_month month = year(2024) / January;
             month != year(2025) / January;
             month += std::chrono::months{1})
        {
            std::cout << std::format("Month : {}", month) << std::endl;
        }
    }
}


namespace Chrono::Years
{
    using namespace std::chrono;

    void Last_Day_of_Month()
    {
        // Last day in a month
        std::chrono::month_day_last last_day_in_feb = February/last;

        std::cout << std::format("last_day_in_feb == {}\n", last_day_in_feb) << std::endl;
        static_assert(std::is_same_v<decltype(last_day_in_feb), std::chrono::month_day_last>);
    }

    void Find_Leaping_Years()
    {   // Last day in a month
        std::chrono::month_day_last last_day_in_feb = February/last;

        // Find leaping years in 2024..2104
        for (std::chrono::year year = 2024y; year <= 2104y; ++year)
        {
            const std::chrono::year_month_day_last maybe_leap = year/last_day_in_feb;
            // same as: year/Februrary/last

            static_assert(std::is_same_v<decltype(maybe_leap), const std::chrono::year_month_day_last>);

            // assert(maybe_leap.ok());
            if (maybe_leap.day() == 29d) { // requires maybe_leap.ok()
                std::cout << std::format("{} is a leap year\n", year);
            }
        }
    }

    void Last_Sunday_of_Year()
    {   // Last weekday in a month
        const std::chrono::month_weekday_last last_sunday = December/Sunday[last];

        // Iterate over the last Sundays in 2024..2030
        for (std::chrono::year year = 2024y; year <= 2030y; ++year) {
            const std::chrono::year_month_weekday_last pseudo = year/last_sunday;
            /** same as: year/December/Sunday[last] **/

            static_assert(std::is_same_v<decltype(pseudo), const std::chrono::year_month_weekday_last>);

            // Convert to the actual year_month_day
            std::chrono::year_month_day actual { pseudo };
            std::cout << std::format("Last Sunday in {} is {}\n", year, actual);
        }
    }

    void Thanksgiving_Days()
    {   // nth weekday in a month US Thanksgiving date, 4th Thursday in November
        const std::chrono::month_weekday thanksgiving = November/Thursday[4]; // ordinal, not index

        for (std::chrono::year year = 2024y; year <= 2030y; year++) {
            // As long as the expression is not ambiguous, the order doesn't matter
            const year_month_day date {thanksgiving/year};
            std::cout << std::format("US thanksgiving in {} is {}\n", year, date);
        }
    }
}

namespace Chrono::Time_To_String
{

    void Asctime()
    {
        std::time_t result = std::time(nullptr);
        std::cout << std::asctime(std::localtime(&result)) << result << " seconds since the Epoch" << std::endl;
    }

    void Localtime()
    {
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        const time_t time = std::chrono::system_clock::to_time_t(now);
        const tm* ltime = std::localtime(&time);

        std::cout << std::put_time(ltime, "%c") << std::endl;
    }

    void PrintTime_One()
    {
        std::time_t t = std::time(nullptr);

        std::cout << std::put_time(std::gmtime(&t), "%c %Z") << '\n';
        std::cout << std::put_time(std::gmtime(&t), "%c") << '\n';
        std::cout << std::put_time(std::localtime(&t), "%c %Z") << '\n';
    }

    void PrintTime_Two()
    {
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        const time_t time = std::chrono::system_clock::to_time_t(now);

        std::cout << std::put_time(std::localtime(&time), "%Y-%m-%d %X") << std::endl;
    }


    constexpr char FORMAT[] { "%d-%02d-%02d %02d:%02d:%02d.%06ld" };

    std::string getCurrentTime(const std::chrono::time_point<std::chrono::system_clock>& timestamp
        = std::chrono::system_clock::now()) noexcept
    {
        const time_t time { std::chrono::system_clock::to_time_t(timestamp) };
        std::tm tm {};
        ::localtime_r(&time, &tm);

        std::string buffer(64, '\0');
        const int32_t size = std::sprintf(buffer.data(), FORMAT,
               tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
               duration_cast<std::chrono::microseconds>(timestamp - time_point_cast<std::chrono::seconds>(timestamp)).count());
        buffer.resize(size);
        buffer.shrink_to_fit();
        return buffer;
    }

    void Time_With_Milliseconds()
    {
        std::cout << getCurrentTime() << std::endl;
    }
}


namespace Chrono::TimeZones
{
    using namespace std::chrono;

    void Test()
    {
        // Time zone database version
        std::string tzdb_version = std::chrono::remote_version();
        std::cout << "Zone database version: " << tzdb_version << std::endl;

        // Iterate over valid timezones
        std::cout << "Zones:\n";
        for (const std::chrono::time_zone& zone : get_tzdb().zones) {
            std::cout << "\t" << zone.name() << std::endl;
        };
        std::cout << std::endl;
    }

    void Get_Time_Zone_By_Name()
    {
        // Locate two time zones in the time zone db that we will be working with
        const std::chrono::time_zone* prague = std::chrono::locate_zone("Europe/Prague");
        const std::chrono::time_zone* newyork = std::chrono::locate_zone("America/New_York");

        std::cout << prague->name() << std::endl;
        std::cout << newyork->name() << std::endl;



        // Local time is unzoned
        std::chrono::local_time meeting { local_days{2025y/March/Wednesday[1]} };
        std::cout << std::format("\nMeeting (local time): {}", meeting) << std::endl;

        // Iterate over all meetings until summer time change
        while (meeting < local_days { 2025y/April/Sunday[1] })
        {
            // create zoned time for Prague
            const zoned_time<seconds> local {prague, local_days{meeting} + 15h};

            // create zoned time for NewYork from the Prague zoned time
            const zoned_time<seconds> remote {newyork, local};

            std::cout << std::format("\t{}: {}\n", prague->name(), local);
            std::cout << std::format("\t{}: {}\n\n", newyork->name(), remote);

            meeting += weeks{1};
        }
    }

    void Zones_Tests()
    {
        time_t rawtime;
        tm* ptm;

        static constexpr int MST = -7;
        static constexpr int UTC = 0;
        static constexpr int CCT = 8;
        static constexpr int UAE = 4;

        time(&rawtime);

        ptm = gmtime(&rawtime);

        puts("Current time around the World:");
        printf("Phoenix, AZ (U.S.)  : %2d:%02d\n", (ptm->tm_hour + MST) % 24, ptm->tm_min);
        printf("Reykjavik (Iceland) : %2d:%02d\n", (ptm->tm_hour + UTC) % 24, ptm->tm_min);
        printf("Beijing (China)     : %2d:%02d\n", (ptm->tm_hour + CCT) % 24, ptm->tm_min);
        printf("Abu Dhabi (UAE)     : %2d:%02d\n", (ptm->tm_hour + UAE) % 24, ptm->tm_min);
    }
}


namespace Chrono::Parse
{
    void SimpleTest()
    {
        const std::string timeString = "2022-07-01 15:30:00";
        std::chrono::time_point<std::chrono::system_clock> timePoint;

        std::istringstream  ss {timeString};
        ss >> std::chrono::parse(std::string {"%Y-%m-%d %H:%M:%S"}, timePoint);

        std::time_t tt = std::chrono::system_clock::to_time_t(timePoint);
        std::cout << "Parsed time: " << std::ctime(&tt);
    }
}


namespace Chrono::Cast_Conversation
{
    void TimePoint_to_Long_and_Back()
    {
        using namespace std::chrono;

        // Get current time with precision of milliseconds | time_point<system_clock, milliseconds>
        const system_clock::time_point currentTime = system_clock::now();

        const int64_t integral_duration = currentTime.time_since_epoch().count();
        const system_clock::time_point currentTimeNew { system_clock::duration { integral_duration } };

        // test
        if (currentTimeNew != currentTime)
            std::cout << "Failure." << std::endl;
        else
            std::cout << "Success." << std::endl;
    }
}

void Chrono::TestAll()
{
    // Time_To_String::Asctime();
    // Time_To_String::Localtime();
    // Time_To_String::PrintTime_One();
    // Time_To_String::PrintTime_Two();
    Time_To_String::Time_With_Milliseconds();

    // Duration::Create_Simple();
    // Duration::Measure_Duration();
    // Duration::HighResolution__PeriodDuration();
    // Duration::SteadyClock__PeriodDuration();
    // Duration::Time_From_Duration();
    // Duration::Zero_Duration();
    // Duration::Min_Max();
    // Duration::DurationCast();
    // Duration::Measure_Duration_Test();

    // Months::Enumerate_Each_Month_of_Year();
    // Months::Month_of_Year();

    // Years::Last_Day_of_Month();
    // Years::Find_Leaping_Years();
    // Years::Last_Sunday_of_Year();
    // Years::Thanksgiving_Days();

    // TimeToString::Test();
    // StringFormat::StrfTime();
    // StringFormat::Asctime();
    // StringFormat::PutTime_To_String();
    // StringFormat::CTime_String();
    // StringFormat::Format();
    // StringFormat::Format2();
    // StringFormat::Format3();
    // StringFormat::StringToTime();

    // Cast_Conversation::TimePoint_to_Long_and_Back();

    // FunctionPerformance::GetCurrentTime_Performance();
    // FunctionPerformance::TestGetCurrentTimeFunctions();

    // TimeZones::Test();
    // TimeZones::Get_Time_Zone_By_Name();
    // TimeZones::Zones_Tests();

    // Parse::SimpleTest();

    // Steady_clock();
    // Clock_Test();
    // High_Resolution_Clock();
    // GM_time_VS_localtime();
    // Time_T();

    // Year_Month_Day();
    // Create_Day_Manually();

    // is_PM_AM();
    // H24_to_AM_PM();

    // TimeOfDay::TimeOfDay_Basics();
    // CalendarDate::Basics();
    // Localtime_TM();

    // Experiments();
};

