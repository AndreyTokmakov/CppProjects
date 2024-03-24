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

namespace Framework
{
    struct DataProvider
    {
        OrderBookBase& orderBook;

        DataProvider(OrderBookBase& book);

        void setStream();

        // TODO: Rename
        void start();
    };

}

#endif //CPPPROJECTS_DATAPROVIDER_H
