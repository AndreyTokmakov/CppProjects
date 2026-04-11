/**============================================================================
Name        : TimeZones.cpp
Created on  : 13.11.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TimeZones.cpp
============================================================================**/

#include "TimeZones.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>
#include <cstdio>
#include <functional>
#include <print>

#include <vector>
#include <set>



namespace TimeZones
{
    using namespace std::chrono;

    void GetVersion_AndAvailableZones() {
        const std::chrono::tzdb &timeZonesDB = std::chrono::get_tzdb();

        const std::string remoteVersion = std::chrono::remote_version();
        std::cout << "Chrono version: " << remoteVersion << std::endl;
        std::cout << "Current DB version: " << timeZonesDB.version << std::endl;

        // Time zone database version

        // Iterate over valid timezones
        std::cout << "Zones:\n";
        for (const std::chrono::time_zone &zone: get_tzdb().zones) {
            std::cout << "\t" << zone.name() << std::endl;
        };
        std::cout << std::endl;
    }

    void GetZones_Databases_List() {
        const std::chrono::tzdb &timeZonesDB = std::chrono::get_tzdb();
        const std::chrono::tzdb_list &dbList = std::chrono::get_tzdb_list();

        const size_t size = std::distance(dbList.begin(), dbList.end());

        std::cout << "Available versions: " << size << std::endl;
        std::cout << std::boolalpha << (&dbList.front() == &timeZonesDB) << std::endl;
    }

    void PrintZones_AndInfo() {   /**
        Zones (db.zones):

            Primary container of time zone information
            Each zone contains complete historical and current rules
            Examples: “America/New_York”, “Europe/London”

        Links (db.links):

            Alternative names for existing zones
            Useful for backward compatibility
            Example: “EST” links to “America/New_York”

        Leap Seconds (db.leap_seconds):

            Records of historical leap second adjustments
            Used for precise astronomical calculations
        **/
        const std::chrono::tzdb &timeZonesDB = std::chrono::get_tzdb();
        const std::chrono::time_point now = std::chrono::system_clock::now();

        std::cout << "Time Zone Database Version: " << timeZonesDB.version << std::endl;
        std::cout << "Number of zones: " << timeZonesDB.zones.size() << std::endl;
        std::cout << "Number of links: " << timeZonesDB.links.size() << std::endl;
        std::cout << "Number of leap seconds: " << timeZonesDB.leap_seconds.size() << std::endl << std::endl;

        for (const auto &zone: timeZonesDB.zones) {
            try {
                const std::chrono::sys_info info = zone.get_info(now);
                std::print("{:<30} offset: {:>6}s  abbrev: {:<6}  ", zone.name(), info.offset.count(), info.abbrev);

                // Show if DST is active
                if (info.save != std::chrono::minutes{0})
                    std::print("DST: {} min", info.save.count());

                std::print("\n");
            }
            catch (const std::exception &e) {
                std::cerr << "Error with zone " << zone.name() << ", " << e.what() << std::endl;
            }
        }
    }

    void Find_Unusual_Zones()
    {   /**
        What makes a time zone unusual? Usually, it’s:
        Non-hour offsets (like UTC+5:45)
        Unusual DST rules (like 30-minute DST changes)
        Here’s our improved code: **/

        const std::chrono::tzdb &timeZonesDB = std::chrono::get_tzdb();
        const std::chrono::time_point now = std::chrono::system_clock::now();

        // Track unique unusual offsets
        std::set<std::chrono::seconds> offsets;

        for (const auto& zone : timeZonesDB.zones) {
            try {
                const std::chrono::sys_info info = zone.get_info(now);

                /** * * * * Check if offset is not a whole hour * * * *  **/
                if (info.offset % std::chrono::hours{1} != std::chrono::seconds{0})
                {
                    offsets.insert(info.offset);

                    std::print("Found unusual zone: {}\n", zone.name());
                    std::print("  Offset: {:+.2f} hours\n",static_cast<double>(info.offset.count()) / 3600.0);
                    std::print("  DST adjustment: {} minutes\n\n",info.save.count());
                }
            }
            catch (const std::exception&) {
                // Skip problematic zones
            }
        }

        std::print("Total unusual offsets found: {}\n", offsets.size());
    }

    struct TimeZoneExplorer
    {
        static void exploreZone(std::string_view zone_name)
        {
            try {
                const std::chrono::time_zone* const zone = locate_zone(zone_name);
                const std::chrono::time_point now = std::chrono::system_clock::now();
                const std::chrono::zoned_time zt {zone, now};
                const std::chrono::sys_info info = zone->get_info(now);

                std::print("\n=== {} ===\n", zone_name);
                std::print("Current time: {:%F %T %Z}\n", zt);
                std::print("Valid period: [{}, {})\n", info.begin, info.end);

                std::print("UTC offset: {}s ({:+.2f} hours)\n",
                           info.offset.count(),
                           static_cast<double>(info.offset.count()) / 3600.0);

                if (0min == info.save) {
                    std::print("DST: No (Standard Time)\n");
                }
                else {
                    std::print("DST: Yes (Saving: {} minutes)\n", info.save.count());
                    const std::chrono::duration standard_offset = info.offset - info.save;
                    std::print("Standard Time offset would be: {}s ({:+.2f} hours)\n",
                               standard_offset.count(),
                               static_cast<double>(standard_offset.count()) / 3600.0);
                }

                std::print("Abbreviation: {} (Note: abbreviations are not unique)\n",info.abbrev);
            }
            catch (const std::exception& exc) {
                std::cerr << "Error exploring " << std::quoted(zone_name) << ". Error: " << exc.what() << std::endl;
            }
        }
    };

    void Exploring_Unusual_Zones()
    {
        TimeZoneExplorer::exploreZone("Asia/Kathmandu");
        TimeZoneExplorer::exploreZone("Australia/Lord_Howe");
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

            meeting += weeks{1U};
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

namespace TimeZones::Print_Time
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


    void TimeZones_Formating()
    {
        const auto now = std::chrono::system_clock::now();
        auto zt_local = std::chrono::zoned_time { std::chrono::current_zone(), now };
        std::print("now is {} UTC and local is: {}\n", now, zt_local);

        constexpr std::string_view Warsaw { "Europe/Warsaw" };
        constexpr std::string_view NewYork { "America/New_York" };
        constexpr std::string_view Tokyo { "Asia/Tokyo" };
        constexpr std::string_view Dubai { "Asia/Dubai" };

        try
        {
            const std::chrono::zoned_time zt_w{Warsaw, now};
            std::print("Warsaw  : {0:%F} {0:%R}\n", zt_w);

            const std::chrono::zoned_time zt_ny{NewYork, now};
            std::print("New York: {0:%F} {0:%R}\n", zt_ny);

            const std::chrono::zoned_time zt_t{Tokyo, now};
            std::print("Tokyo   : {0:%F} {0:%R}\n", zt_t);

            const std::chrono::zoned_time zt_d{Dubai, now};
            std::print("Dubai   : {0:%F} {0:%R}\n", zt_d);
        }
        catch (std::runtime_error& ex)
        {
            std::print("Error: {}", ex.what());
        }
    }

    void Daylight_Saving_Time ()
    {
        try
        {
            const auto now = std::chrono::floor<std::chrono::minutes>(std::chrono::system_clock::now());
            auto zt_local = std::chrono::zoned_time{ "Asia/Dubai", now };
            std::print("now is {} UTC and Dubai is: {}\n", now, zt_local);

            auto info = zt_local.get_info();
            std::print("local time info: \nabbrev: {},\n begin {}, end {}, \noffset {}, save {}\n",
                       info.abbrev, info.begin, info.end, info.offset, info.save);
        }
        catch (std::runtime_error& ex)
        {
            std::print("Error: {}", ex.what());
        }
    }

    void printInfo(std::chrono::sys_days sd,
                   std::string_view zone)
    {
        auto zt_local = std::chrono::zoned_time{ zone, std::chrono::sys_days{sd} };
        auto info = zt_local.get_info();
        std::print("time info for {:%F} in {}:\nabbrev: {},\nbegin {}, end {}, \noffset {}, save {}\n",
                   sd, zone, info.abbrev, info.begin, info.end, info.offset, info.save);
    }

    void Print_TimeZope_Info()
    {
        try
        {
            printInfo(std::chrono::year{ 2024 } / 9 / 14, "Asia/Dubai");
            printInfo(std::chrono::year{ 2024 } / 11 / 14, "Asia/Dubai");
        }
        catch (std::runtime_error& ex)
        {
            std::print("Error: {}", ex.what());
        }
    }
}


void TimeZones::TestAll()
{
    // TimeZones::GetVersion_AndAvailableZones();
    // TimeZones::GetZones_Databases_List();
    // TimeZones::PrintZones_AndInfo();
    // TimeZones::Find_Unusual_Zones();
    TimeZones::Exploring_Unusual_Zones();

    // TimeZones::Get_Time_Zone_By_Name();
    // TimeZones::Zones_Tests();


    // Print_Time::Print_Formating();
    // Print_Time::TimeZones_Formating();
    // Print_Time::Daylight_Saving_Time();
    // Print_Time::Print_TimeZope_Info();
}