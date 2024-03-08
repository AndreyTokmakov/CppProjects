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
#include <format>

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
    constexpr std::string_view dataFilePathPartDebug {"../../FinTechMarketProjects/data/orders_part_debug.csv" };
    constexpr std::string_view dataFile_Test1 {"../../FinTechMarketProjects/data/orders_test_1.csv" };


    template<typename K, typename V, typename Comparator>
    std::ostream& operator<<(std::ostream& stream, const std::map<K, V, Comparator>& map)
    {
        for (const auto & [k,v]: map)
            stream << '(' << k << ", " << v << ')' << std::endl;
        return stream;
    }

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

        [[nodiscard]]
        inline size_t size() const noexcept {
            return buyOrders.size() + sellOrders.size();
        }
    };

    class OrderBook
    {
        std::unordered_map<std::string, SymbolOrders> orderBook;
        std::unordered_map<uint64_t, SymbolOrders::OrderIter> ordersById;

    public:

        void cancelOrder(const Order& order,
                         SymbolOrders& ordersBySymbol)
        {
            if (const auto iter = ordersById.find(order.id); ordersById.end() != iter)
            {
                SymbolOrders::OrdersList& orders = ordersBySymbol.getOrders(order.side);
                orders.erase(iter->second);
                ordersById.erase(iter);
            }
            else {
                std::cerr << "Error: Failed to Cancel order with " << order.id << ". Not found\n";
            }
        }

        void modifyOrder(const Order& order,
                         SymbolOrders& ordersBySymbol)
        {
            if (auto orderIter = ordersById.find(order.id); ordersById.end() != orderIter)
            {   /** Price has changed **/
                if (order.price != orderIter->second->second.price)
                {
                    SymbolOrders::OrdersList& orders = ordersBySymbol.getOrders(order.side);
                    if (auto orderNode = orders.extract(orderIter->second); orderNode)
                    {
                        orderNode.key() = order.price;
                        orderNode.mapped() = order;
                        auto [iter, b, c] = orders.insert(std::move(orderNode));
                    } else {
                        std::cerr << "Error: Failed to update order. Extract order node error\n";
                    }
                }
                else {
                    orderIter->second->second.volume = order.volume;
                }

                // TODO: Try to find match | Do the trade
            }
            else {
                std::cerr << "Error: Failed to Amend order with " << order.id << ". Not found\n";
            }
        }

        void addOrder(Order&& order,
                      SymbolOrders& ordersBySymbol)
        {
            SymbolOrders::OrdersList& ordersToMatch = ordersBySymbol.getOppositeOrders(order.side);

            // TODO: Try to find match | Do the trade

            SymbolOrders::OrdersList& orders = ordersBySymbol.getOrders(order.side);
            if (auto [iter, inserted] = orders.try_emplace(order.price); inserted) {
                ordersById[order.id] = iter;
                iter->second = std::move(order);
            }
            else {
                std::cout << "Duplicate price "; printOrder(order);
                for (const auto & [k, v]: orders) {
                    std::cout << "            " << k << "  "; printOrder(v);
                }
            }
        }

        void processOrder(Order&& order)
        {
            SymbolOrders &ordersBySymbol = orderBook[order.symbol];
            if (Operation::Cancel == order.operation) {
                cancelOrder(order, ordersBySymbol);
            } else if (Operation::Amend == order.operation) {
                modifyOrder(order, ordersBySymbol);
            } else if (Operation::Insert == order.operation) {
                addOrder(std::move(order), ordersBySymbol);
            }
        }

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
            if (std::fstream file {path }; file.is_open() && file.good())
            {
                Order order;
                std::string line;
                std::vector<std::string_view> params;
                while (std::getline(file, line))
                {
                    split_to(line, params);
                    parseOrder(params, order);

                    // TODO: Check if its OK to std::move order here ???
                    processOrder(std::move(order));
                }
            }
        }

        friend struct Tester;
    };

    struct Tester
    {
        const OrderBook& orderBook;

        explicit Tester(const OrderBook& book): orderBook { book } {}

        void printBook()
        {
            for (const auto & [symbol, orders] : orderBook.orderBook)
            {
                std::cout << "-------------------------- " << symbol << " ----------------------------------\n";
                std::cout << "Buy:\n";
                for (const auto& [price, order] : orders.buyOrders)
                    std::cout << "\tPrice:" << price << ", Volume: " << order.volume << std::endl;
                std::cout << "Sell:\n";
                for (const auto& [price, order] : orders.sellOrders)
                    std::cout << "\tPrice:" << price << ", Volume: " << order.volume << std::endl;
            }
        }
    };
}

namespace Experiments
{
    void printMaps(const std::map<double, uint16_t, std::less<>>& ordersOne,
                   const std::map<double, uint16_t, std::less<>>& ordersTwo)
    {
        constexpr size_t width {10};
        auto iterOne = ordersOne.cbegin();
        auto iterTwo = ordersTwo.cbegin();

        auto printOrder = [](const auto it) {
            std::cout << std::left << std::setfill(' ') << std::setw(width)
                << std::format("({}, {})", it->first, it->second);
        };

        while (!(ordersOne.cend() == iterOne && ordersTwo.cend() == iterTwo))
        {
            if (ordersOne.cend() != iterOne) {
                printOrder(iterOne);
                ++iterOne;
            } else {
                std::cout << std::left << std::setfill(' ') << std::setw(width) << "";
            }

            std::cout << "      ";

            if (ordersTwo.cend() != iterTwo) {
                printOrder(iterTwo);
                ++iterTwo;
            } else {
                std::cout << std::left << std::setfill(' ') << std::setw(width) << "";
            }

            std::cout << std::endl;
        }
    }


    void Foo()
    {
        std::map<double, uint16_t, std::less<>> sellOrders{
                {10.0, 3},
                {15.0, 5},
                {17.0, 5},
                {17.9, 7},
                {20.0, 8},
                {25.0, 3}
        };

        std::map<double, uint16_t, std::less<>> buyOrders{
                {17.5, 10},
        };

        printMaps(buyOrders, sellOrders);


        for (auto buyIter = buyOrders.begin(); buyIter != buyOrders.end(); ++buyIter)
        {
            for (auto sellIter = sellOrders.begin(); sellIter != sellOrders.end() && buy.second;) {
                if (buyIter->second >= sellIter->second) {
                    buyIter->second -= sellIter->second;
                    sellOrders.erase(sellIter++);
                } else {
                    sellIter->second -= buy.second;
                    buy.second = 0;
                }
            }
        }

        std::cout << std::endl;
        printMaps(buyOrders, sellOrders);
    }
}

void OrderBookNew::TestAll()
{
    // TODO: Buy and Sell order Maps need to be sorted in different way

    /*
    OrderBook book;
    book.readData(dataFile_Test1);

    Tester tester {book};
    tester.printBook();
    */

    Experiments::Foo();

}