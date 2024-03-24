/**============================================================================
Name        : Types.h
Created on  : 16.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Types
============================================================================**/

#ifndef PRICEPOINTBOOK_TYPES_H
#define PRICEPOINTBOOK_TYPES_H

#include <cstdint>
#include <string>
#include <vector>

namespace Types
{
    using id_type = uint64_t;
    using quantity_type = float;
    using volume_type = quantity_type;
    using price_type = float;

    enum class OrderSide : char {
        Buy,
        Sell
    };

    enum class Operation : char {
        Insert = 'I',
        Cancel = 'C',
        Amend  = 'A'
    };


    template <typename T1, typename T2> requires
            std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
    struct NamedPair
    {
        T1 price {};
        T2 quantity {};
    };

    // TODO: Is it needed ???
    using RawEvent = std::string;

    // TODO: Is it needed ???
    using PriceLevel = NamedPair<price_type, quantity_type>;

    // TODO: Is it needed ???
    struct Event
    {
        /** ignoring the field in the context of this task: outside the scope **/
        // id_type timestamp {};

        // TODO: can we use std::array<char, 6> instead of std::string ???
        std::string symbol {};

        /** ignoring the field in the context of this task: outside the scope **/
        id_type firstID { 0 };

        /** ignoring the field in the context of this task: outside the scope **/
        id_type lastID { 0 };

        std::vector<PriceLevel> buyOrders {};
        std::vector<PriceLevel> sellOrders {};
    };


    // TODO: Is it needed ???

    // FIXME: Check alignment  ????
    // FIXME: ---> should fit to ONE CacheLine !!!
    // FIXME: ---> CacheLine size validation
    struct Order
    {
        uint64_t timestamp {};
        std::string symbol {}; // TODO: can we use std::array<char, 6> ??
        id_type id { 0 };
        volume_type volume { 0 };
        price_type price { 0.0 };
        Operation operation { Operation::Insert };
        OrderSide side { OrderSide::Buy };
    };

    // TODO: Is it needed ???
    struct Snapshot
    {
        /** ignoring the field in the context of this task: outside the scope **/
        // id_type lastUpdateId {0};

        std::vector<Order> bids;
        std::vector<Order> asks;
    };
}

#endif //PRICEPOINTBOOK_TYPES_H