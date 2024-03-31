/**============================================================================
Name        : MatchingOrderBookEx.h
Created on  : 01.04.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : MatchingOrderBookEx
============================================================================**/

#include "MatchingOrderBookEx.h"

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
#include <map>
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

namespace MatchingOrderBookEx
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

    // The order of fields in the raw INSERT order
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

    // The order of fields in the raw AMEND order
    struct AmendOrderFields final {
        static constexpr size_t OperationType {0};
        static constexpr size_t OrderID {1};
        static constexpr size_t Price {2};
        static constexpr size_t Volume {3};

        AmendOrderFields() = delete;
        ~AmendOrderFields() = delete;
    };

    // The order of fields in the raw PULL order
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
        using OrderSymbol = std::string;
        using OrdersList  = std::list<Order>; // TODO: To priority queue??
        using TradesList  = std::vector<Trade>;
        using OrderIter   = typename OrdersList::iterator;
        using RawOrderStore = std::array<std::string_view, rawOrderPartsMAX>;

        // POD structure containing all the orders broken down by type: BUY and SELL
        struct SymbolOrders final {
            OrdersList buyOrders {};
            OrdersList sellOrders {};

            [[nodiscard]]
            OrdersList& getOrders(OrderSide side) noexcept {
                return (OrderSide::Buy == side) ? buyOrders : sellOrders;
            }

            [[nodiscard]]
            const OrdersList& getOrders(OrderSide side) const noexcept {
                return (OrderSide::Buy == side) ? buyOrders : sellOrders;
            }

            [[nodiscard]]
            OrdersList& getOppositeOrders(OrderSide side) noexcept {
                return (OrderSide::Buy == side) ? sellOrders : buyOrders;
            }

            [[nodiscard]]
            const OrdersList& getOppositeOrders(OrderSide side) const noexcept  {
                return (OrderSide::Buy == side) ? sellOrders : buyOrders;
            }

            [[nodiscard]]
            bool empty() const noexcept {
                return buyOrders.empty() && sellOrders.empty();
            }
        };

        /** To map the order ID to a structure containing -> [Order symbol, Iterator for the order data] **/
        std::unordered_map<OrderIDType, OrderIter> orderIdMapping {};

        /** TODO: Change --> std::map<K, V> for sorted list **/
        /** To map an order symbol to a structure containing all orders for this symbol: both SELL and BUY **/
        std::unordered_map<OrderSymbol, SymbolOrders> ordersBySymbol {};

        TradesList tradesCompleted {};

        // TODO: Use concepts if there is time left: default_ctor, floating_point, std::itegral
        template<typename T>
        [[nodiscard]]
        static T extractType(std::string_view str) {
            T result {};
            std::from_chars(str.data(), str.data() + str.size(), result);
            return result;
        }

        // The method is a very naive (but sufficient for this task) stub that returns something like a
        // timestamp: in this case, an incremented statistical value is returned.
        // Used to account for the order/priority
        // ***IMPORTANT***: do not use in multithreaded mode
        [[nodiscard("Dont call it without reason. Static interment is not so cheap")]]
        static size_t getTimestamp() noexcept {
            static size_t timestamp {0};
            return ++timestamp;
        }

        // TODO: Refactor
        size_t findMatches(Order& originOrder) {
            if (auto iterOrder = ordersBySymbol.find(originOrder.symbol); ordersBySymbol.end() != iterOrder) {
                SymbolOrders& symbolOrders { iterOrder->second };
                /** Get orders list for side: BUY or SELL **/
                OrdersList& oppositeOrders = symbolOrders.getOppositeOrders(originOrder.side);
                oppositeOrders.sort(OrderComparator);

                for (auto iter = oppositeOrders.begin(); oppositeOrders.end() != iter; ) {
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
                            /** Remove record for order ID: **/
                            orderIdMapping.erase(matchingOrder.id);
                            /** Remove matched order data itself: **/
                            oppositeOrders.erase(iter++);
                        } else {
                            originOrder.volume = 0;
                            return 0;
                        }
                    }
                    else ++iter;
                }
            }
            return originOrder.volume;
        }

        void HandleInsertOrder(const RawOrderStore& parts)
        {
            // TODO: here is a bunch of redundant setters: can I put it into the method later sometime?
            Order order {};
            order.symbol = parts[InsertOrderFields::Symbol];
            order.id = extractType<size_t>(parts[InsertOrderFields::OrderID]);
            order.price = extractType<double>(parts[InsertOrderFields::Price]);
            order.volume = extractType<size_t>(parts[InsertOrderFields::Volume]);
            order.side = getOrderSide(parts[InsertOrderFields::Side]);
            order.timestamp = getTimestamp();

            if (0 == findMatches(order)) {
                // A match was found for the order and its size became zero - no need to add it to the book
                return;
            }

            // TODO: use 'try_emplace' instead 'emplace'
            const auto [iterOrders, _] = ordersBySymbol.emplace(order.symbol, SymbolOrders{});
            if (const auto [iter, inserted] = orderIdMapping.emplace(order.id, nullptr); inserted) {
                OrdersList& orders = iterOrders->second.getOrders(order.side);
                /** Move/Insert the Order data to the List and get iterator pointing to the inserted value **/
                iter->second = orders.insert(orders.end(), std::move(order));
            } else {
                // TODO: Duplicate order ID received! Exception???
            }
        }

        void HandleAmendOrder(const RawOrderStore& parts) {
            const size_t orderId { extractType<size_t>(parts[AmendOrderFields::OrderID])};
            if (const auto iter = orderIdMapping.find(orderId); orderIdMapping.end() != iter) {
                /** Ref to Order data stored in list: **/
                Order& order { *iter->second };

                const auto price { extractType<double>(parts[AmendOrderFields::Price]) };
                const auto volume { extractType<size_t>(parts[AmendOrderFields::Volume]) };

                // TODO: exit of only 'volume' decreased'
                /** Only the volume is decreased. **/
                if (!(order.price == price && order.volume >= volume))
                    order.timestamp = getTimestamp();

                order.price = price;
                order.volume = volume;

                if (0 == findMatches(order)) {
                    SymbolOrders& symbolOrders = ordersBySymbol.find(order.symbol)->second;
                    /** Get orders list for side: BUY or SELL **/
                    OrdersList& orders = symbolOrders.getOrders(order.side);
                    /** Delete the order data from list using iterator **/
                    orders.erase(iter->second);
                    /** Delete the order ID <--> [symbol, iterator] mapping **/
                    orderIdMapping.erase(iter);
                }
            } else {
                // TODO: AMEND order/request received for non-existing order! Exception???
            }
        }

        void HandlePullOrder(const RawOrderStore& parts) {
            const size_t orderId { extractType<size_t>(parts[PullOrderFields::OrderID])};
            if (const auto iter = orderIdMapping.find(orderId); orderIdMapping.end() != iter) {
                /** Ref to Order data stored in list: **/
                const Order& order { *iter->second };
                SymbolOrders& symbolOrders = ordersBySymbol.find(order.symbol)->second;
                /** Get orders list for side: BUY or SELL **/
                OrdersList& orders = symbolOrders.getOrders(order.side);
                /** Delete the order data from list using iterator **/
                orders.erase(iter->second);
                /** Delete the order ID <--> [symbol, iterator] mapping **/
                orderIdMapping.erase(iter);
                // TODO: Delete SymbolOrders is SymbolOrders::empty() == TRUE
            } else {
                // TODO: PULL order/request received for non-existing order *! Exception???
            }
        }

        // TODO: Test for performance
        void handleOrderRaw(std::string_view rawOrder)
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

        void processOrders(const std::vector<std::string>& input) {
            for (const auto& rawOrder: input)
                handleOrderRaw(rawOrder);
        }

        //===========================================================================================

        std::string double2String(double value) {
            std::string str = std::to_string(value);
            size_t pos = str.size();
            for (size_t i = 0, len = str.size(); i < str.length(); ++i) {
                if ('0' != str[len - 1 - i] && '.' != str[len - 1 - i] )
                    break;
                --pos;
            }
            str.resize(pos);
            return str;
        }

        std::vector<std::string> buildResult()
        {
            std::vector<std::string> result;

            for (const Trade& trade: tradesCompleted) {
                std::string& str = result.emplace_back(trade.orderSymbol);
                str.append(1, ',').append(double2String(trade.buyPrice));
                str.append(1, ',').append(std::to_string(trade.volume));
                str.append(1, ',').append(std::to_string(trade.aggressiveOrderId));
                str.append(1, ',').append(std::to_string(trade.passiveOrderId));
            }

            /*for (const auto& [k, v]: ordersBySymbol)
            {
                std::string& str = result.emplace_back("===" + k + "===");
                const OrdersList& buyOrders = v.buyOrders;
                for (const Order& order: buyOrders) {
                    //str.append()
                }

                const OrdersList& sellOrders = v.sellOrders;
                for (const Order& order: sellOrders) {
                }
            }*/

            for (const auto& s: result) std::cout << s << std::endl;
            return result;
        }

        std::vector<std::string> buildResult2()
        {
            // lambda to sum up the quantities of all orders with the same price
            auto listPriceReduce = [](OrdersList& orderList) {
                for (auto iter = std::next(orderList.begin()); orderList.end() != iter;) {
                    auto prev = std::prev(iter);
                    if (prev->price == iter->price) {
                        prev->volume += iter->volume;
                        orderList.erase(iter++);
                    } else
                        ++iter;
                }
            };

            auto lessPriceComparator = [](const Order& a, const Order& b) noexcept {
                return a.price < b.price;
            };

            auto greaterPriceComparator = [](const Order& a, const Order& b) noexcept {
                return a.price > b.price;
            };

            std::vector<std::string> result {};
            result.reserve(tradesCompleted.size());
            for (const Trade& trade: tradesCompleted) {
                std::string& str = result.emplace_back(trade.orderSymbol);
                str.append(1, ',').append(double2String(trade.buyPrice));
                str.append(1, ',').append(std::to_string(trade.volume));
                str.append(1, ',').append(std::to_string(trade.aggressiveOrderId));
                str.append(1, ',').append(std::to_string(trade.passiveOrderId));
            }

            for (auto& [k, v]: ordersBySymbol) {
                OrdersList& buyOrders = v.buyOrders;
                OrdersList& sellOrders = v.sellOrders;

                buyOrders.sort(greaterPriceComparator);
                sellOrders.sort(lessPriceComparator);
                listPriceReduce(buyOrders);
                listPriceReduce(sellOrders);

                [[maybe_unused]]
                std::string& str = result.emplace_back("===" + k + "===");

                auto buyIter = buyOrders.begin(), sellIter = sellOrders.begin();
                while (!(buyOrders.end() == buyIter && sellOrders.end() == sellIter))
                {
                    std::string& line = result.emplace_back();
                    if (buyOrders.end() == buyIter)
                        line.append(1, ',');
                    else {
                        line.append(double2String(buyIter->price)).append(1, ',').append(std::to_string(buyIter->volume));
                        ++buyIter;
                    }

                    line.append(1, ',');

                    if (sellOrders.end() == sellIter)
                        line.append(2, ',');
                    else {
                        line.append(double2String(sellIter->price)).append(1, ',').append(std::to_string(sellIter->volume));
                        ++sellIter;
                    }
                }
            }

            for (const auto& s: result) std::cout << s << std::endl;
            return result;
        }

        //===========================================================================================


        void SortLists() {
            for (auto& [k, v]: ordersBySymbol) {
                v.sellOrders.sort(OrderComparator);
                v.buyOrders.sort(OrderComparator);
            }
        }

        void info() {
            for (const auto& [k, v]: ordersBySymbol) {
                std::cout << "===" << k << "===" << std::endl;
                const SymbolOrders& orders = v;
                std::cout << "-------------- Buy ----------------:\n";
                for (const Order& order: orders.buyOrders)
                    order.print();
                std::cout << "-------------- Sell ----------------:\n";
                for (const Order& order: orders.sellOrders)
                    order.print();

            }
        }

        void printID_Mapping() {
            for (const auto& [k, v]: orderIdMapping) {
                std::cout << k << " ";
                v->print();
            }
        }

        void printTrades() {
            for (const Trade& trade: tradesCompleted) {
                std::cout << trade.orderSymbol << ","
                          << trade.buyPrice << ","
                          << trade.volume << ","
                          << trade.aggressiveOrderId << ","
                          << trade.passiveOrderId
                          // << "                " << trade.sellPrice
                          << std::endl;
            }
        }

        void printContent() {
            for (const auto& [k, v]: ordersBySymbol) {
                std::cout << "===" << k << "===" << std::endl;
                const SymbolOrders& orders = v;
                for (const Order& order: orders.buyOrders) {
                    std::cout << order.price << "  " << order.volume << "   BUY" <<std::endl;
                }
                for (const Order& order: orders.sellOrders){
                    std::cout << order.price << "  " << order.volume << "   SELL" <<std::endl;
                }
            }
        }
    };
}


namespace MatchingOrderBookEx::Tests
{
    void InsertSimple() {
        OrderBook book{};
        std::vector<std::string> orders {
                "INSERT,1,AAPL,BUY,12.2,5",
        };

        book.processOrders(orders);

        book.printTrades();
        book.printContent();


        // REQUIRE(result.size() == 2);
        // CHECK(result[0] == "===AAPL===");
        // CHECK(result[1] == "12.2,5,,");
    }

    void InsertSimple_TestOutput() {
        OrderBook book{};
        std::vector<std::string> orders {
                "INSERT,1,AAPL,BUY,12.2,5",
        };

        book.processOrders(orders);
        book.buildResult();


        // REQUIRE(result.size() == 2);
        // CHECK(result[0] == "===AAPL===");
        // CHECK(result[1] == "12.2,5,,");
    }


    void InsertTests() {
        std::vector<std::string> orders {
                "INSERT,1,AAPL1,BUY,11.2,5",
                "INSERT,2,AAPL2,BUY,21.2,51",
                "INSERT,5,AAPL1,SELL,12.2,51",
                "INSERT,3,AAPL1,BUY,13.2,5",
                "INSERT,4,AAPL2,BUY,22.4,52",
                "INSERT,8,AAPL2,SELL,23.4,52",
        };

        OrderBook book{};
        book.processOrders(orders);

        // book.info();
        book.SortLists();
        book.printID_Mapping();
    }

    void simple_Match() {
        std::vector<std::string> orders {
                "INSERT,1,AAPL,BUY,12.2,5",
                "INSERT,2,AAPL,SELL,12.1,8",
        };

        OrderBook book{};
        book.processOrders(orders);

        // book.printTrades();
        // book.info();
        book.buildResult();

        // CHECK(result[0] == "AAPL,12.2,5,2,1");
        // CHECK(result[1] == "===AAPL===");
        // CHECK(result[2] == ",,12.1,3");
    }

    void Pull_Orders() {
        std::vector<std::string> orders {
                "INSERT,1,AAPL1,BUY,11.2,5",
                "INSERT,2,AAPL1,BUY,21.2,7",
                "PULL,1",
        };

        OrderBook book{};
        book.processOrders(orders);


        book.printID_Mapping();
        //book.info();
    }

    void Amend_Orders() {
        std::vector<std::string> orders {
                "INSERT,1,AAPL1,BUY,11.2,5",
                "INSERT,2,AAPL1,BUY,21.2,7",
                "AMEND,2,123.456,321",
        };

        OrderBook book{};
        book.processOrders(orders);

        book.printID_Mapping();
        book.info();
    }

    void Find_Matches_Insert() {
        std::vector<std::string> orders {
                "INSERT,1,AAPL1,BUY,52.2,5",
                "INSERT,5,AAPL1,SELL,51.2,11",
                "INSERT,2,AAPL1,BUY,51.3,7",
        };

        OrderBook book{};
        book.processOrders(orders);
    }

    void Simple_MatchTest() {
        std::vector<std::string> orders {
                "INSERT,1,AAPL,BUY,12.2,5",
                "INSERT,2,AAPL,SELL,12.1,8",
        };

        OrderBook book{};
        book.processOrders(orders);

        // book.printTrades();
        book.printContent();
        book.buildResult();

        /*
        CHECK(result[0] == "AAPL,12.2,5,2,1");
        CHECK(result[1] == "===AAPL===");
        CHECK(result[2] == ",,12.1,3");
         */
    }

    void Simple_MatchTest_2() {
        std::vector<std::string> orders {
                "INSERT,1,AAPL,BUY,14.235,5",
                "INSERT,2,AAPL,BUY,14.235,6",
                "INSERT,3,AAPL,BUY,14.235,12",
                "INSERT,4,AAPL,BUY,14.234,5",
                "INSERT,5,AAPL,BUY,14.23,3",
                "INSERT,6,AAPL,SELL,14.237,8",
                "INSERT,7,AAPL,SELL,14.24,9",

                //"INSERT,11,AAPL,SELL,123.24,9",
                //"INSERT,12,AAPL,SELL,123.24,9",

                "PULL,1",
                "INSERT,8,AAPL,SELL,14.234,25",
        };

        OrderBook book{};
        book.processOrders(orders);

        // book.printTrades();
        // book.buildResult();
        book.buildResult2();
        book.printContent();

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

    // multi symbol
    void Simple_MatchTest_3() {
        std::vector<std::string> orders {
                "INSERT,1,WEBB,BUY,0.3854,5",
                "INSERT,2,TSLA,BUY,412,31",
                "INSERT,3,TSLA,BUY,410.5,27",
                "INSERT,4,AAPL,SELL,21,8",
                "INSERT,11,WEBB,SELL,0.3854,4",
                "INSERT,13,WEBB,SELL,0.3853,6",
        };

        OrderBook book{};
        book.processOrders(orders);

        // book.printTrades();
        // book.printContent();
        book.buildResult2();

        /*
        Process finished with exit code 0
        3,1");
        CHECK(result[2] == "===AAPL===");
        CHECK(result[3] == ",,21,8");
        CHECK(result[4] == "===TSLA===");
        CHECK(result[5] == "412,31,,");
        CHECK(result[6] == "410.5,27,,");
        CHECK(result[7] == "===WEBB===");
        CHECK(result[8] == ",,0.3853,5");
         * */
    }

    // multi symbol
    void Simple_MatchTest_Amend() {
        std::vector<std::string> orders {
                "INSERT,1,WEBB,BUY,45.95,5",
                "INSERT,2,WEBB,BUY,45.95,6",
                "INSERT,3,WEBB,BUY,45.95,12",
                "INSERT,4,WEBB,SELL,46,8",
                "AMEND,2,46,3",
        };

        OrderBook book{};
        book.processOrders(orders);

        book.printTrades();
        book.info();

        // CHECK(result[0] == "WEBB,46,3,2,4");
    }

    // multi symbol
    void Simple_MatchTest_4() {
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

        OrderBook book{};
        book.processOrders(orders);


        book.buildResult2();

        /*
        REQUIRE(result.size() == 6);
        CHECK(result[0] == "WEBB,46,3,2,4");
        CHECK(result[1] == "WEBB,45.95,1,5,1");
        CHECK(result[2] == "WEBB,45.95,1,6,1");
        CHECK(result[3] == "WEBB,45.95,1,7,3");
        CHECK(result[4] == "===WEBB===");
        CHECK(result[5] == "45.95,16,46,5");
         * */
    }
}

void MatchingOrderBookEx::TestAll()
{
    // Tests::InsertSimple();
    // Tests::InsertSimple_TestOutput();
    // Tests::InsertTests();
    // Tests::simple_Match();
    // Tests::Pull_Orders();
    // Tests::Amend_Orders();
    // Tests::Find_Matches_Insert();

    // Tests::Simple_MatchTest();
    // Tests::Simple_MatchTest_2();
    // Tests::Simple_MatchTest_3();
    Tests::Simple_MatchTest_4();

    // Tests::Simple_MatchTest_Amend();
}





















