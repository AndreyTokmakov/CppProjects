/**============================================================================
Name        : PostgreSQL.cpp
Created on  : 05.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "PostgreSQL.h"

#include <iostream>
#include <string_view>
#include <utility>
#include <vector>
#include <thread>
#include <fstream>
#include <format>
#include <print>

#if 0
#include <pqxx/pqxx>

namespace Tests
{
    void queryTest()
    {
        pqxx::connection pgConnection {"postgresql://admin@localhost/snapshots"};
        pqxx::work transaction { pgConnection };

        const pqxx::result resultSet = transaction.exec("select * from employees;");

        std::println("Result set size = {}", resultSet.size());
        for (const pqxx::row& row: resultSet)
        {
            //std::print("Row size: {} | ", row.size());
            for (const auto& col: row) {
                std::cout <<  col << " | ";
            }
            std::cout << std::endl;
        }

        pgConnection.close();
    }
}
#endif


void PostgreSQL::TestAll()
{

}
