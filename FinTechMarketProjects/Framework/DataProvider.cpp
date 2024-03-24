/**============================================================================
Name        : DataProvider.cpp
Created on  : 24.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DataProvider.cpp
============================================================================**/

#include "DataProvider.h"
#include <iostream>
#include <fstream>

namespace Framework
{
    DataProvider::DataProvider(OrderBookBase& book): orderBook {book} {
    }

    void DataProvider::setStream(const std::filesystem::path& filePath)
    {
        // TODO: Implement
        path = filePath;
    }

    // TODO: Renames
    void readData()
    {

    }

    void DataProvider::start()
    {
        // Read from stream
        // TODO: Match Order_ID <---> Worker_ID ??? Or in

        std::cout << std::filesystem::exists(path) << std::endl;

        if (std::ifstream file { path }; file.is_open() && file.good())
        {
            std::string line;
            while (std::getline(file, line))
            {
                // split_to(line, params);
                // parseOrder(params, order);

                orderBook.handleEvent(line);

                // TODO: Check if its OK to std::move order here ???
                // processOrder(std::move(order));
            }
        }
    }
}