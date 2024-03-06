/**============================================================================
Name        : OrderBookNew.cpp
Created on  : 06.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : OrderBookNew.cpp
============================================================================**/

#include "OrderBookNew.h"

#include <iostream>
#include <string_view>
#include <vector>
#include <filesystem>
#include <fstream>
#include <map>
#include <unordered_map>
#include <charconv>

/*
    I = Insert / new order - his is a new order added to the book; it will have a new/unique order-id.
    C = Cancel / delete order: The order with the given order-id is to be removed from the book.
    A = Amend  / modify order :The order with the given order-id is to be changed to the new volume and/or price.

    timestamp;symbol;order-id;operation;side;volume;price

    For example:
    14:17:21.877391;DVAM1;00000001;I;BUY;100;12.5
    14:17:22.123523;DVAM1;00000002;I;SELL;37;13.5
    14:17:22.343883;DVAM1;00000001;A;BUY;100;12.7
    14:17:24.737292;DVAM1;00000003;I;SELL;37;13.3
    14:17:24.893811;DVAM1;00000004;I;BUY;55;12.7
    14:17:25.883711;DVAM1;00000002;C;SELL;37;13.5
*/

namespace
{
    constexpr std::string_view dataFilePath {"../../FinTechMarketProjects/data/orders1.dat" };
    constexpr std::string_view dataFilePathPart {"../../FinTechMarketProjects/data/orders_part.csv" };

    void split_to(const std::string &str,
                  std::vector<std::string_view>& params,
                  const char delimiter = ';')
    {
        params.clear();
        size_t pos = 0, prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos) {
            params.emplace_back(str.data() + prev, pos - prev);
            prev = pos + 1;
        }
        params.emplace_back(str.data() + prev, str.length() - prev);
    }
}

namespace OrderBookNew
{
    enum class OrderSide : char {
        Buy,
        Sell
    };

    enum class Operation : char {
        Insert = 'I',
        Cancel = 'C',
        Amend  = 'A'
    };

    // FIXME: Check alignment  ????
    // FIXME: ---> should fit to ONE CacheLine !!!
    // FIXME: ---> CacheLine size validation
    struct Order
    {   // FIXME: to size_t ???
        uint64_t timestamp {};
        std::string symbol {};
        uint64_t id { 0 };
        uint32_t volume { 0 };
        float price { 0.0 };
        Operation operation { Operation::Insert };
        OrderSide side { OrderSide::Buy };
    };

    void printOrder(const Order& order)
    {
        std::cout << "Order("
                  << "\n\ttimestamp: " << order.timestamp
                  << "\n\tsymbol: " << order.symbol
                  << "\n\tid: " << order.id
                  << "\n\toperation: " << static_cast<char>(order.operation)
                  << "\n\tside: " << (order.side == OrderSide::Buy ? "BUY" : "SELL")
                  << "\n\tvolume: " << order.volume
                  << "\n\tprice: " << order.price
                  <<"\n)\n";
    }

    struct SymbolOrders final
    {
        using OrdersList = std::map<double, Order>;
        using OrderIter = typename OrdersList::iterator;

        OrdersList buyOrders {};
        OrdersList sellOrders {};

        // SymbolOrders() { std::cout << "SymbolOrders" << std::endl; }

        [[nodiscard]]
        inline OrdersList& getOrders(OrderSide side) noexcept {
            return (OrderSide::Buy == side) ? buyOrders : sellOrders;
        }

        [[nodiscard]]
        inline const OrdersList& getOrders(OrderSide side) const noexcept {
            return (OrderSide::Buy == side) ? buyOrders : sellOrders;
        }

        [[nodiscard]]
        inline bool empty() const noexcept {
            return buyOrders.empty() && sellOrders.empty();
        }

        [[nodiscard]]
        inline size_t size() const noexcept {
            return buyOrders.size() + sellOrders.size();
        }
    };

    struct OrderBook
    {
        std::unordered_map<std::string, SymbolOrders> orderBook;
        std::unordered_map<uint64_t, SymbolOrders::OrderIter> ordersById;

        void processOrder(const Order& order)
        {
            SymbolOrders &ordersBySymbol = orderBook[order.symbol];
            SymbolOrders::OrdersList& orders = ordersBySymbol.getOrders(order.side);

            if (Operation::Cancel == order.operation)
            {
                // TODO: Check if we can replace find() with erase()
                if (const auto iter = ordersById.find(order.id); ordersById.end() != iter) {
                    orders.erase(iter->second);
                    ordersById.erase(iter);
                }
                else {
                    std::cerr << "Cancel order failed: No order with " << order.id << " found\n";
                }
            }
            else if (Operation::Amend == order.operation)
            {
                if (auto orderIter = ordersById.find(order.id); ordersById.end() != orderIter) {
                    // Price has changed
                    if (order.price != orderIter->second->second.price) {
                        orders.erase(orderIter->second);
                        auto [iter, ok] = orders.try_emplace(order.price, std::move(order));
                        orderIter->second = iter;
                    }
                    else {
                        orderIter->second->second.volume = order.volume;
                    }
                }
                else {
                    std::cerr << "ERROR Amend" << std::endl;
                    printOrder(order);
                }
            }
            else if (Operation::Insert == order.operation)
            {
                // TODO: Try to find match here  ???

                auto [iter, ok] = orders.try_emplace(order.price);
                if (ok) {
                    ordersById[order.id] = iter;
                    iter->second = std::move(order);
                    // debugCounter++;
                }
                else {
                    std::cout << "Duplicate price "; printOrder(order);
                    for (const auto & [k, v]: orders) {
                        std::cout << "            " << k << "  "; printOrder(v);
                    }
                }
            }
        }
    };

    bool parseOrder(std::vector<std::string_view>& params,
                    Order& order)
    {
        //order.timestamp.assign(params[0].data(), params[0].size());
        order.symbol.assign(params[1].data(), params[1].size());
        std::from_chars(params[2].data(), params[2].data() + params[2].size(), order.id);

        switch (params[3][0]) {
            case 'I': order.operation = Operation::Insert; break;
            case 'A': order.operation = Operation::Amend; break;
            case 'C': order.operation = Operation::Cancel; break;
        }

        switch (params[4][0]) {
            case 'S': order.side = OrderSide::Sell; break;
            case 'B': order.side = OrderSide::Buy; break;
        }

        std::from_chars(params[5].data(), params[5].data() + params[5].size(), order.volume);
        std::from_chars(params[6].data(), params[6].data() + params[6].size(), order.price);

        // for (auto& s: params) std::cout << s << ' '; std::cout << std::endl;
        return true;
    }


    // TODO: Renames
    void readData(const std::filesystem::path& path)
    {
        OrderBook orderBook;
        if (std::fstream file {path}; file.is_open() && file.good())
        {
            Order order;
            std::string line;
            std::vector<std::string_view> params;
            while (std::getline(file, line))
            {
                split_to(line, params);
                parseOrder(params, order);
                orderBook.processOrder(order);
            }
        }
    }
}

void OrderBookNew::TestAll()
{
    OrderBookNew::readData(dataFilePathPart);

    // std::cout << sizeof(Order) << std::endl;
    // std::cout << std::hardware_destructive_interference_size << std::endl;
    // std::cout << sizeof(uint64_t) << std::endl;
    // std::cout << sizeof(std::string) << std::endl;
}