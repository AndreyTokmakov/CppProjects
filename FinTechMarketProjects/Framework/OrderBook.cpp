/**============================================================================
Name        : OrderBook.cpp
Created on  : 24.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : OrderBook.cpp
============================================================================**/

#include <iostream>
#include "OrderBook.h"


// TODO:
//  - Пул потоков
//  - Очередь для каждого воркера


namespace Framework
{
    void OrderBookBase::handleEvent(const Types::RawEvent & event)
    {
        std::cout << "OrderBookBase: " << event << std::endl;
    }

    void OrderBook::handleEvent(const RawEvent &event)
    {
        std::cout << "OrderBook: " << event << std::endl;
    }
}