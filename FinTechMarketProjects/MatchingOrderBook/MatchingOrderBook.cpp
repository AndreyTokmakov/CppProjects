/**============================================================================
Name        : MatchingOrderBook.cpp
Created on  : 01.04.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : PointsAndLInes
============================================================================**/

#include "MatchingOrderBook.h"

#include <iostream>
#include <charconv>
#include <string_view>
#include <vector>
#include <deque>
#include <ostream>
#include <list>
#include <array>
#include <functional>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <concepts>

namespace {
    inline constexpr std::string_view buyOrderString { "BUY" };
    inline constexpr std::string_view sellOrderString { "SELL" };

    inline constexpr std::string_view insertTypeString { "INSERT" };
    inline constexpr std::string_view amendTypeString { "AMEND" };
    inline constexpr std::string_view pullTypeString { "PULL" };
}

/*
 * Brief description of the application structure:
 *
 * 1. All orders for one symbol (BUY and SELL) are stored in the 'SymbolOrders' structure
 *
 * 2. To map the orders symbol's and all orders for it, the std::map structure 'ordersBySymbol' is used
 *    Reasons:
 *    - In order to have an effective [symbol -> all orders] lookup speed
 *    - TO have a unique list of symbols of all active orders in sorted way (for final output)
 *
 * 3. To map the order ID to the data of the order itself, std::unordered_map is used.
 *    Reasons:
 *    - In order to have an effective [order ID -> Order Data] lookup speed: O (1)
 *    - To process PULL and DAMNED requests as quickly as possible
 *
 * To store orders by symbol with the same order-side the std::list is used.
 * The use of the std::list collection is caused by the following reasons
 * 1. effective removal of elements in the middle
 * 2. iterators with any manipulation with std::list remain valid
 *
 */

namespace MatchingOrderBook
{
    enum class OrderSide {
        None, // Possible error to be reported
        Buy,
        Sell
    };

    enum class OperationType  {
        None,
        Insert,
        Pull,
        Amend,
    };

    [[nodiscard]]
    OrderSide getOrderSide(std::string_view method) {
        if (buyOrderString == method)
            return OrderSide::Buy;
        else if (sellOrderString == method)
            return OrderSide::Sell;
        return OrderSide::None;
    }

    // TODO: Check for perf?? switch_case()
    [[nodiscard]]
    OperationType getOrderType(std::string_view type) {
        if (insertTypeString == type)
            return OperationType::Insert;
        else if (amendTypeString == type)
            return OperationType::Amend;
        else if (pullTypeString == type)
            return OperationType::Pull;
        return OperationType::None;
    }

    std::string orderTypeToString(OperationType type) {
        switch (type) {
            case OperationType::Insert: return "Insert"; break;
            case OperationType::Amend: return "Amend"; break;
            case OperationType::Pull: return "Pull"; break;
            case OperationType::None: return "None"; break;
        }
        return std::string{"None"};
    }

    std::string orderSideToString(OrderSide size) {
        switch (size) {
            case OrderSide::Sell: return std::string{"Sell"}; break;
            case OrderSide::Buy: return std::string{"Buy"}; break;
            case OrderSide::None: return std::string{"None"}; break;
        }
        return std::string{"Err"};
    }

    struct InsertOrderFields final {
        static constexpr size_t OperationType {0};
        static constexpr size_t OrderID {1};
        static constexpr size_t Symbol {2};
        static constexpr size_t Side {3};
        static constexpr size_t Price {4};
        static constexpr size_t Volume {5};

        InsertOrderFields() = delete;
        ~InsertOrderFields() = delete;
    };

    struct AmendOrderFields final {
        static constexpr size_t OperationType {0};
        static constexpr size_t OrderID {1};
        static constexpr size_t Price {2};
        static constexpr size_t Volume {3};

        AmendOrderFields() = delete;
        ~AmendOrderFields() = delete;
    };

    struct PullOrderFields final {
        static constexpr size_t OperationType {0};
        static constexpr size_t OrderID {1};

        PullOrderFields() = delete;
        ~PullOrderFields() = delete;
    };


    [[nodiscard]]
    double str2Number(const std::string& str) {
        return std::strtod(str.data(), nullptr);
    }


    struct Order final {
        size_t id {0};
        std::string symbol {};
        OrderSide side { OrderSide::Buy };
        // TODO: --> int?
        size_t volume {0};
        double price {0};
        size_t timestamp {0};

        void print() const {
            std::cout << "[id: " << id << ", symbol: " << symbol <<
                      ", volume: " << volume << ", price: " << price
                      << ", timestamp: " << timestamp << ", Side: " << orderSideToString(side)
                      << "]\n";
        }
    };

    struct Trade final {
        std::string orderSymbol{};
        size_t aggressiveOrderId {0};
        size_t passiveOrderId {0};
        double sellPrice{0};
        double buyPrice{0};
        size_t volume {0};

        Trade(const std::string &symbol, size_t aggressiveId, size_t passiveID,
              double sPrice, double bPrice, size_t vol) :
                orderSymbol {symbol}, aggressiveOrderId {aggressiveId}, passiveOrderId{passiveID},
                sellPrice{sPrice}, buyPrice{bPrice}, volume {vol} {
        }
    };


    auto OrderComparator = [](const Order& a, const Order& b) noexcept {
        if (a.price == b.price)
            return a.timestamp < b.timestamp;
        else
            return a.price > b.price;
    };


    struct OrderBook
    {
        static inline constexpr size_t rawOrderPartsMAX { 6 };

        using OrderIDType = size_t;
        using OrdersList = std::list<Order>; // TODO: To priority queue??
        using TradesList = std::vector<Trade>;
        using ListIter = typename OrdersList::iterator;
        using OrderIter = std::pair<ListIter, OrdersList&>;
        using RawOrderStore = std::array<std::string_view, rawOrderPartsMAX>;

        std::unordered_map<OrderIDType, OrderIter> cache {};
        OrdersList buyOrders {};
        OrdersList sellOrders {};
        TradesList tradesCompleted {};


        // TODO: Concepts: default_ctor, floating_point, std::itegral
        template<typename T>
        [[nodiscard]]
        T extractType(std::string_view str) {
            T result {};
            std::from_chars(str.data(), str.data() + str.size(), result);
            return result;
        }

        // TODO: Add description
        [[nodiscard("Dont call it without reason. Static interment is not so cheap")]]
        static size_t getTimestamp() noexcept {
            static size_t timestamp {0};
            return ++timestamp;
        }

        // TODO: Refactor
        size_t findMatches(Order& originOrder) {
            OrdersList& oppositeOrders { (OrderSide::Buy == originOrder.side) ? sellOrders : buyOrders };
            oppositeOrders.sort(OrderComparator); // TODO: add symbol to sort

            // TODO: Stop when order volume done
            // TODO: Stop when found first lower/greate price for order
            // TODO: Stop for other symbol
            for (auto iter = oppositeOrders.begin(); oppositeOrders.end() != iter;) {
                if (iter->symbol != originOrder.symbol) {
                    ++iter;
                    continue;
                }

                Order& matchingOrder { *iter };
                if ((OrderSide::Sell == originOrder.side && matchingOrder.price >= originOrder.price) ||
                    (OrderSide::Buy == originOrder.side && originOrder.price >= matchingOrder.price))
                {
                    const size_t tradeVolume = std::min(matchingOrder.volume, originOrder.volume);
                    tradesCompleted.emplace_back(originOrder.symbol, originOrder.id, matchingOrder.id,
                                                 originOrder.price, matchingOrder.price, tradeVolume);

                    originOrder.volume -= tradeVolume;
                    matchingOrder.volume -= tradeVolume;

                    if (0 == matchingOrder.volume) {
                        /** Remove cache record for order ID: **/
                        cache.erase(matchingOrder.id);
                        /** Remove matched order data itself: **/
                        oppositeOrders.erase(iter++);
                    } else {
                        originOrder.volume = 0;
                        return 0;
                    }
                }
                else ++iter;
            }

            return originOrder.volume;
        }

        void HandleInsertOrder(const RawOrderStore& parts)
        {
            Order order {};
            order.symbol = parts[InsertOrderFields::Symbol];
            order.id = extractType<size_t>(parts[InsertOrderFields::OrderID]);
            order.price = extractType<double>(parts[InsertOrderFields::Price]);
            order.volume = extractType<size_t>(parts[InsertOrderFields::Volume]);
            order.side = getOrderSide(parts[InsertOrderFields::Side]);
            order.timestamp = getTimestamp();

            if (0 == findMatches(order))
                return;

            const OrderSide side { getOrderSide(parts[InsertOrderFields::Side]) };
            OrdersList& orders = (OrderSide::Buy == side) ? buyOrders : sellOrders;
            if (const auto [iter, inserted] = cache.emplace(order.id, OrderIter {nullptr, orders}); inserted) {
                /** Move/Insert the Order data to the List and get iterator pointing to the inserted value **/
                iter->second.first = orders.insert(orders.end(), std::move(order));
            } else {
                // TODO: Duplicate order ID received! Exception???
            }
        }

        void HandleAmendOrder(const RawOrderStore& parts) {
            const size_t orderId { extractType<size_t>(parts[AmendOrderFields::OrderID])};
            if (const auto iter = cache.find(orderId); cache.end() != iter) {
                /** Ref to Order data stored in list: **/
                Order& order { *iter->second.first};

                const auto price { extractType<double>(parts[AmendOrderFields::Price]) };
                const auto volume { extractType<size_t>(parts[AmendOrderFields::Volume]) };

                /** Only the volume is decreased. **/
                if (!(order.price == price && order.volume >= volume))
                    order.timestamp = getTimestamp();

                order.price = price;
                order.volume = volume;

                if (0 == findMatches(order)) {
                    OrdersList& orders = iter->second.second;
                    /** Delete the order data from list using iterator **/
                    orders.erase(iter->second.first);
                    /** Delete record from cache **/
                    cache.erase(iter);
                }
            } else {
                // TODO: AMEND order/request received for non-existing order! Exception???
            }
        }

        void HandlePullOrder(const RawOrderStore& parts) {
            const size_t orderId { extractType<size_t>(parts[PullOrderFields::OrderID])};
            if (const auto iter = cache.find(orderId); cache.end() != iter) {
                OrdersList& orders = iter->second.second;
                /** Delete the order data from list using iterator **/
                orders.erase(iter->second.first);
                /** Delete record from cache **/
                cache.erase(iter);
            } else {
                // TODO: PULL order/request received for non-existing order *! Exception???
            }
        }

        // TODO: Test for performance
        void parseOrderString(std::string_view rawOrder)
        {
            RawOrderStore parts {};
            size_t pos = 0, prev = 0, idx = 0;
            while ((pos = rawOrder.find(',', prev)) != std::string::npos) {
                parts[idx++] = rawOrder.substr(prev, pos - prev);
                prev = pos + 1;
            }
            parts[idx++] = rawOrder.substr(prev, rawOrder.length() - prev);

            switch (const OperationType type = getOrderType(parts[InsertOrderFields::OperationType]); type) {
                case OperationType::Insert: HandleInsertOrder(parts);break;
                case OperationType::Amend:  HandleAmendOrder(parts); break;
                case OperationType::Pull :  HandlePullOrder(parts);  break;
                default: /** Error? **/ break;
            }
        }

        //===========================================================================================

        void info() {
            std::cout << " ------------------------ Cache ----------------------------\n";
            for (const auto& [k, v]: cache)
                std::cout << k << " = {"
                          << v.first->id  << ", " << v.first->symbol  << ", "
                          << v.first->price  << ", " << v.first->volume  << "} "
                          << " Size = " << v.second.size()
                          << std::endl;

            std::cout << " ------------------------ Buy orders: ----------------------------\n";
            for (const auto& order: buyOrders)
                order.print();

            std::cout << " ------------------------ Sell orders: ----------------------------\n";
            for (const auto& order: sellOrders)
                order.print();
        }

        void printTrades() {
            for (const Trade& trade: tradesCompleted) {
                std::cout << trade.orderSymbol << ","
                        << trade.buyPrice << ","
                        << trade.volume << ","
                        << trade.aggressiveOrderId << ","
                        << trade.passiveOrderId
                        << "                "
                        << trade.sellPrice
                        << std::endl;
            }
        }
    };
}


namespace MatchingOrderBook::Tests
{
    void simple_Match() {
        OrderBook book{};
        std::vector<std::string> orders {
            "INSERT,1,AAPL,BUY,12.2,5",
            "INSERT,2,AAPL,SELL,12.1,8",
        };

        for (const auto& o: orders)
            book.parseOrderString(o);

        book.printTrades();
        book.info();

        /*
        CHECK(result[0] == "AAPL,12.2,5,2,1");
        CHECK(result[1] == "===AAPL===");
        CHECK(result[2] == ",,12.1,3");
         */
    }

    void Test1() {
        OrderBook book{};
        std::vector<std::string> orders {
                "INSERT,2,AAPL,BUY,22.45,12",
                "INSERT,4,AAPL,BUY,23.45,15",
                "INSERT,5,AAPL,BUY,23.45,15",
                "INSERT,14,AAPB,BUY,21.45,11",
                "INSERT,44,AAPB,BUY,121.45,11",
                "INSERT,11,AAPS,SELL,11.45,21",
                //"AMEND,14,21.45,11",
                //"PULL,4",
                //"AMEND,14,22.45,11",
                //"AMEND,14,23.45,11",
        };

        /*
        book.parseOrderString(orders[0]);
        book.parseOrderString(orders[1]);
        book.parseOrderString(orders[2]);

        book.parseOrderString(orders[3]);
        book.parseOrderString(orders[4]);
         */
        for (const auto& o: orders)
            book.parseOrderString(o);
        book.info();
    }

    void Test2() {
        OrderBook book{};
        std::vector<std::string> orders {
                "INSERT,1,AAPL,BUY,14.235,5",
                "INSERT,2,AAPL,BUY,14.235,6",
                "INSERT,3,AAPL,BUY,14.235,12",
                "INSERT,4,AAPL,BUY,14.234,5",
                "INSERT,5,AAPL,BUY,14.23,3",
                "INSERT,6,AAPL,SELL,14.237,8",
                "INSERT,7,AAPL,SELL,14.24,9",
                "PULL,1",
                "INSERT,8,AAPL,SELL,14.234,25",
        };

        for (const auto& o: orders)
            book.parseOrderString(o);

        book.printTrades();
        book.info();

        /*
        REQUIRE(result.size() == 7);
        CHECK(result[0] == "AAPL,14.235,6,8,2");
        CHECK(result[1] == "AAPL,14.235,12,8,3");
        CHECK(result[2] == "AAPL,14.234,5,8,4");
        CHECK(result[3] == "===AAPL===");
        CHECK(result[4] == "14.23,3,14.234,2");
        CHECK(result[5] == ",,14.237,8");
        CHECK(result[6] == ",,14.24,9");
         * */
    }


    void Test3() {
        OrderBook book{};
        std::vector<std::string> orders {
                "INSERT,1,WEBB,BUY,0.3854,5",
                "INSERT,2,TSLA,BUY,412,31",
                "INSERT,3,TSLA,BUY,410.5,27",
                "INSERT,4,AAPL,SELL,21,8",
                "INSERT,11,WEBB,SELL,0.3854,4",
                "INSERT,13,WEBB,SELL,0.3853,6"
        };


        for (const auto& o: orders)
            book.parseOrderString(o);

        // book.info();
        book.printTrades();

        /*
        REQUIRE(result.size() == 9);
        CHECK(result[0] == "WEBB,0.3854,4,11,1");
        CHECK(result[1] == "WEBB,0.3854,1,13,1");
        CHECK(result[2] == "===AAPL===");
        CHECK(result[3] == ",,21,8");
        CHECK(result[4] == "===TSLA===");
        CHECK(result[5] == "412,31,,");
        CHECK(result[6] == "410.5,27,,");
        CHECK(result[7] == "===WEBB===");
        CHECK(result[8] == ",,0.3853,5");
        */
    }

    void Test3_1() {
        OrderBook book{};
        std::vector<std::string> orders {
                "INSERT,1,WEBB,BUY,0.3854,5",
                "INSERT,11,WEBB,SELL,0.3854,4",
                "INSERT,13,WEBB,SELL,0.3853,6"
        };


        for (const auto& o: orders)
            book.parseOrderString(o);


        book.printTrades();
        book.info();

        /*
        REQUIRE(result.size() == 9);
        CHECK(result[0] == "WEBB,0.3854,4,11,1");
        CHECK(result[1] == "WEBB,0.3854,1,13,1");
        CHECK(result[2] == "===AAPL===");
        CHECK(result[3] == ",,21,8");
        CHECK(result[4] == "===TSLA===");
        CHECK(result[5] == "412,31,,");
        CHECK(result[6] == "410.5,27,,");
        CHECK(result[7] == "===WEBB===");
        CHECK(result[8] == ",,0.3853,5");
        */
    }

    void Test_BuyTest() {
        OrderBook book{};
        std::vector<std::string> orders {
            "INSERT,1,WEBB,BUY,45.95,5",
            "INSERT,3,WEBB,BUY,45.95,12",
            "INSERT,4,WEBB,SELL,46,8",
            "INSERT,2,WEBB,BUY,46,3",
        };


        for (const auto& o: orders)
            book.parseOrderString(o);

        book.printTrades();
        book.info();
    }

    void Test_AmendTest_Simple() {
        OrderBook book{};
        std::vector<std::string> orders {
            "INSERT,1,WEBB,BUY,45.95,5",
            "INSERT,2,WEBB,BUY,45.95,6",
            "INSERT,3,WEBB,BUY,45.95,12",
            "INSERT,4,WEBB,SELL,46,8",
            "AMEND,2,46,3",
        };

        for (const auto& o: orders)
            book.parseOrderString(o);

        book.printTrades();
        book.info();
    }

    void Test_AmendTest_2() {
        OrderBook book{};
        std::vector<std::string> orders {
                "INSERT,1,WEBB,BUY,45.95,5",
                "INSERT,2,WEBB,BUY,45.95,6",
                "INSERT,3,WEBB,BUY,45.95,12",
                "INSERT,4,WEBB,SELL,46,8",
                "AMEND,2,46,3",
                "INSERT,5,WEBB,SELL,45.95,1",
                "AMEND,1,45.95,3",
                "INSERT,6,WEBB,SELL,45.95,1",
                "AMEND,1,45.95,5",
                "INSERT,7,WEBB,SELL,45.95,1",
        };

        for (const auto& o: orders)
            book.parseOrderString(o);

        book.printTrades();
        //book.info();

        /*
        CHECK(result[0] == "WEBB,46,3,2,4");
        CHECK(result[1] == "WEBB,45.95,1,5,1");
        CHECK(result[2] == "WEBB,45.95,1,6,1");
        CHECK(result[3] == "WEBB,45.95,1,7,3");
        CHECK(result[4] == "===WEBB===");
        CHECK(result[5] == "45.95,16,46,5");
         */
    }
}

void MatchingOrderBook::TestAll()
{
    Tests::simple_Match();
    // Tests::Test2();
    // Tests::Test3();
    // Tests::Test3_1();
    // Tests::Test_BuyTest();
    // Tests::Test_AmendTest_Simple();
    // Tests::Test_AmendTest_2();
}





















