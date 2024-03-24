/**============================================================================
Name        : OrderBook.h
Created on  : 24.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : OrderBook.h
============================================================================**/

#ifndef CPPPROJECTS_ORDERBOOK_H
#define CPPPROJECTS_ORDERBOOK_H

#include "Types.h"

namespace Framework
{
    using namespace Types;

    // TODO: Not sure we need inheritance here

    struct OrderBookBase
    {
        virtual void handleEvent(const RawEvent& event);
        virtual ~OrderBookBase() = default;
    };

    struct OrderBook: OrderBookBase
    {
        void handleEvent(const RawEvent& event) override;
    };
};

#endif //CPPPROJECTS_ORDERBOOK_H
