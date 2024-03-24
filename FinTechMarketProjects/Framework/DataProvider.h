/**============================================================================
Name        : DataProvider.h
Created on  : 24.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DataProvider.h
============================================================================**/

#ifndef CPPPROJECTS_DATAPROVIDER_H
#define CPPPROJECTS_DATAPROVIDER_H

#include "OrderBook.h"

#include <filesystem>

namespace Framework
{
    struct DataProvider
    {
        OrderBookBase& orderBook;

        std::filesystem::path path;

        DataProvider(OrderBookBase& book);

        void setStream(const std::filesystem::path& path);

        // TODO: Rename
        void start();
    };

}

#endif //CPPPROJECTS_DATAPROVIDER_H
