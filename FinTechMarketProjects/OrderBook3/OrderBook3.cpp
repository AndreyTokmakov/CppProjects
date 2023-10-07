/**============================================================================
Name        : OrderBook3.h
Created on  : 01.04.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : OrderBook3
============================================================================**/

#include "OrderBook3.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string_view>
#include <map>
#include <unordered_map>
#include <charconv>
#include <cstdint>


/*
    I = Insert / new order - his is a new order added to the book; it will have a new/unique order-id.
    C = Cancel / delete order: The order with the given order-id is to be removed from the book.
    A = Amend / modify order :The order with the given order-id is to be changed to the new volume and/or price.

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
    inline constexpr std::string_view dataFilePath {
            R"(/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/FinTechMarketProjects/data/orders_test1.dat)" };

}


namespace OrderBook3
{
    [[nodiscard]]
    static std::vector<std::string> split(std::string_view str) {
        std::vector<std::string> params {};
        params.reserve(7);
        size_t pos = 0, prev = 0;
        while ((pos = str.find(';', prev)) != std::string::npos) {
            params.emplace_back(str, prev, pos - prev);
            prev = pos + 1;
        }
        params.emplace_back(str, prev, str.length() - prev);
        return params;
    }

    enum class OrderSide {
        Buy,
        Sell
    };

    enum class Operation : char {
        Insert = 'I',
        Cancel = 'C',
        Amend  = 'A'
    };

    struct Order {
        std::string timestamp {}; // TODO: to int
        std::string id {};
        std::string symbol {};
        uint32_t volume {0};
        double price {0};
        OrderSide side { OrderSide::Buy };
        Operation operation { Operation::Insert };
    };

    [[nodiscard]]
    Operation getOperation(std::string_view op) {
        return static_cast<Operation>(op.front());
    }

    [[nodiscard]]
    OrderSide getSide(std::string_view method) {
        return "BUY" == method ? OrderSide::Buy : OrderSide::Sell;
    }

    [[nodiscard]]
    double str2Number(const std::string& str) {
        return std::strtod(str.data(), nullptr);
    }

    // TODO: Concepts: default_ctor, floating_point, std::itegral
    template<typename T>
    [[nodiscard]]
    T extractType(std::string_view str) {
        T result {};
        std::from_chars(str.data(), str.data() + str.size(), result);
        return result;
    }

    // TODO: Debug
    void printOrder(const Order& order) {
        std::cout << order.timestamp << "|"  << order.symbol << "|" << order.id  << "|";

        if (Operation::Insert == order.operation) {
            std::cout << 'I' << '|';
        } else if (Operation::Cancel == order.operation) {
            std::cout << 'C' << '|';
        } else if (Operation::Amend == order.operation) {
            std::cout << 'A' << '|';
        }

        if (OrderSide::Buy == order.side) {
            std::cout << "BUY" << '|';
        } else{
            std::cout << "SELL" << '|';
        }

        std::cout << order.volume << "|" << order.price << std::endl;
    }

    // TODO: Check for move (std::string_view --> std::string&&)
    Order parseOrder(std::string_view rawOrder) {
        std::vector<std::string> params = split(rawOrder);

        // TODO: move ???
        Order order {};
        order.timestamp = std::move(params[0]);
        order.symbol = std::move(params[1]);
        order.id = std::move(params[2]);
        order.operation = getOperation(params[3]);
        order.side = getSide(params[4]);
        order.volume = extractType<uint32_t>(params[5]);
        order.price = str2Number(params[6]);

        // std::cout << rawOrder << std::endl;
        // printOrder(order);

        return order;
    }


    // [price, volume]
    using OrdersList = std::map<double, uint32_t>;

    struct SymbolOrders final {
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
        inline OrdersList& getOppositeOrders(OrderSide side) noexcept {
            return (OrderSide::Buy == side) ? sellOrders : buyOrders;
        }

        [[nodiscard]]
        inline const OrdersList& getOppositeOrders(OrderSide side) const noexcept  {
            return (OrderSide::Buy == side) ? sellOrders : buyOrders;
        }

        [[nodiscard]]
        inline bool empty() const noexcept {
            return buyOrders.empty() && sellOrders.empty();
        }
    };


    class OrderBook
    {
    public:

        std::unordered_map<std::string, SymbolOrders> orderBook;

        void processRawOrder(std::string_view rawOrder)
        {
            const Order order = parseOrder(rawOrder);

            // TODO: of use [] operator
            const auto [iter, ok] = orderBook.try_emplace(order.symbol);

            // TODO: Handle 'Cancel' and 'Delete/Amend

            // TODO: Find match
            // A match was found for the order and its size became zero - no need to add it to the book

            OrdersList& orders = iter->second.getOrders(order.side);

            if (Operation::Insert == order.operation) {
                orders[order.price] += order.volume;
            }
        }

        void readOrders(std::string_view path)
        {
            std::vector<std::string> rawOrders {};
            if (std::fstream file = std::fstream(path.data()); file.is_open() && file.good()) {
                while (std::getline(file, rawOrders.emplace_back())) { /* */ }
            }

            for (const auto& str: rawOrders) {
                if (7 != split(str).size()){
                    std::cout << "Error" << std::endl;
                }
            }
        }

        void printBook() {
            for (const auto& [symbol, orders]: orderBook) {
                std::cout << "------------------- " << symbol << "------------------------------\n";

                const OrdersList& sellOrders = orders.sellOrders;
                if (!sellOrders.empty()) {
                    std::cout << "Sell orders:\n";
                    for (const auto &[price, vol]: sellOrders) {
                        std::cout << "   " << price << " <-> " << vol << std::endl;
                    }
                }

                const OrdersList& buyOrders = orders.buyOrders;
                if (!buyOrders.empty()) {
                    std::cout << "Buy orders:\n";
                    for (const auto &[price, vol]: buyOrders) {
                        std::cout << "   " << price << " <-> " << vol << std::endl;
                    }
                }
            }
        }

        void readOrders2(std::string_view path)
        {
            std::string line;
            if (std::fstream file = std::fstream(path.data()); file.is_open() && file.good()) {
                while (std::getline(file, line)) {
                    processRawOrder(line); // TODO: Move
                }
            }


        }
    };

}

void OrderBook3::TestAll()
{
    OrderBook book;

    // book.readOrders(dataFilePath);
    book.readOrders2(dataFilePath);
    book.printBook();

}