/**============================================================================
Name        : DataProvider.cpp
Created on  : 24.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DataProvider.cpp
============================================================================**/

#include "DataProvider.h"

namespace Framework
{
    DataProvider::DataProvider(OrderBookBase& book): orderBook {book} {
    }

    void setStream()
    {
        // TODO: Implement
    }


    void DataProvider::start()
    {
        // Read from stream
        // TODO: Match Order_ID <---> Worker_ID ??? Or in

        for (int i = 0; i < 3; ++i)
        {
            orderBook.handleEvent("Event_" + std::to_string(i));
        }
    }
}