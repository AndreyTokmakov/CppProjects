/**============================================================================
Name        : main.cpp
Created on  : 05.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Databases
============================================================================**/

#include <iostream>
#include <vector>
#include <string_view>

#include "MySQL/MySQL.h"
#include "MySQL/MySqlX.h"
#include "MySQL/SQLite.h"
#include "PostgreSQL/PostgreSQL.h"
#include "ClickHouse/ClickhouseClient.h"

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    MySQL::TestAll();
    // MySqlX::TestAll();
    // SQLite::TestAll();
    // PostgreSQL::TestAll();
    // ClickhouseClient::TestAll();

    return EXIT_SUCCESS;
}

