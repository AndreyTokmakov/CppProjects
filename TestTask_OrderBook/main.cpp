//============================================================================
// Name        : main.cpp
// Created on  : 03.04.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : 
//============================================================================

#include <iostream>
#include <string_view>
#include <vector>
#include <deque>
#include <ostream>
#include <array>
#include <fstream>
#include <unordered_map>
#include <chrono>

namespace OrderBookImpl
{
    inline constexpr std::string_view buyOrderString { "BUY" };
    inline constexpr std::string_view sellOrderString { "SELL" };

    enum class OrderType {
        None, // Possible error to be reported
        Buy,
        Sell
    };

    enum class Operation : char {
        None = '\0',
        Insert = 'I',
        Cancel = 'C',
        Amend  = 'A'
    };

    [[nodiscard]]
    OrderType getOrderType(std::string_view method) {
        if (buyOrderString == method)
            return OrderType::Buy;
        else if (sellOrderString == method)
            return OrderType::Sell;
        return OrderType::None;
    }

    [[nodiscard]]
    Operation getOperation(std::string_view method) {
        return static_cast<Operation>(method.front());
    }

    [[nodiscard]]
    double str2Number(const std::string& str) {
        return std::strtod(str.data(), nullptr);
    }

    struct Order final {
        /** TODO: 'timestamp' type --> size_t or something more suitable to store timestamp **/
        std::string timestamp {};
        std::string id {};
        /** To improve look up speed. 'idHash' will be used instead 'id' to match orders. **/
        size_t idHash {0};
        double volume {0};
        double price {0};

        Order() = default;

        // INFO: All strings in the example fits string SSO --> no use for move
        Order(const std::string& time,
              const std::string& id,
              size_t idHash,
              const std::string& volume,
              const std::string& price):
                timestamp {time}, id {id}, idHash {idHash}, volume {str2Number(volume)}, price {str2Number(price)} {
        }
    };

    std::ostream& operator<<(std::ostream& stream, const Order& order) {
        stream << "Order [id: " << order.id << ", Price: "
               << order.price << ", Volume: "
               << order.volume << ", Timestamp: "
               << order.timestamp << "]";
        return stream;
    }

    struct Orders {
        // [ std::vector VS std::deque ]
        // vector showed a little better performance (2-3%) in given test data regardless of the
        // significant amount of deletion of elements in the middle (not on the back) of the vector
        //
        // Most likely this is due to the fact that the list (vector) of orders did not grow much
        // and deleting orders and the ~center did not affect performance --> based on the fact that
        // there will probably be more such orders stored in collection, I think it's better to use std::deque
        using OrdersList = std::deque<Order>;

        OrdersList buyOrders {};
        OrdersList sellOrders {};

        [[nodiscard]]
        size_t getOrdersTotal() const noexcept {
            return buyOrders.size() + sellOrders.size();
        }
    };


    class OrderBook  {
    private:
        std::unordered_map<std::string, Orders> book {};

        /** OrderID hashing strategy: P.S. we should use DI here instead. **/
        constexpr static std::hash<std::string> hasher { std::hash<std::string>{} };

        /** Order's objects comparator: **/
        constexpr static auto greaterByVolume = [](const Order& x, const Order& y) {
            return x.volume > y.volume ;
        };

        constexpr static size_t maxTopOrdersCount { 3 };
        constexpr static size_t orderParts { 7 };

        /** For csv file column matching: **/
        struct OrderFields final {
            static constexpr size_t Timestamp {0};
            static constexpr size_t Symbol {1};
            static constexpr size_t OrderID {2};
            static constexpr size_t Operation {3};
            static constexpr size_t Side {4};
            static constexpr size_t Volume {5};
            static constexpr size_t Price {6};

            OrderFields() = delete;
            ~OrderFields() = delete;
        };

    public:
        bool readOrders(std::string_view path) {
            std::vector<std::string> ordersData {};
            if (std::fstream file = std::fstream(path.data()); file.is_open() && file.good()) {
                while (std::getline(file, ordersData.emplace_back())) { /* */ }
            } else {
                return false;
            }

            if (ordersData.back().empty())
                ordersData.pop_back();
            parseOrders(ordersData);
            return true;
        }


    public:
        /**
         * Counts the total number of orders per symbol.
         * Prints out (in the freestyle) the total number of orders (buy + sell) for each symbol
         * @return  None
         */
        void OrderCounts() const {
            for (const auto& [symbol, orders]: book) {
                std::cout << symbol << "  " << orders.getOrdersTotal() << std::endl;
            }
        }

        /**
         * Finds the top 3 biggest BUY orders in terms of volume for a specific symbol.
         *
         * Outputs to the console (std::cout) a list of 3 buy orders with the maximum volume.
         * If the list of orders is less than 3, then all available orders are printed.
         * if no orders are found for this symbol - prints nothing
         *
         * min_heap approach is used to efficiently search for N_Top records
         *
         * @param  symbol - value of the order symbol (originator/client)
         * @return None
         */
        void BiggestBuyOrders(const std::string& symbol) const
        {
            if (const auto iter = book.find(symbol); book.end() != iter) {
                const Orders::OrdersList & buyOrders = iter->second.buyOrders;
                std::cout << "Top " << maxTopOrdersCount << " BUY orders for '" << symbol << "'\n";
                if (maxTopOrdersCount >= buyOrders.size()) {
                    /** There is no enough orders - display all that we have: **/
                    std::for_each(buyOrders.cbegin(), buyOrders.cend(), [](const auto& o) {
                        std::cout << "   " << o << std::endl;
                    });
                    return;
                }

                /** Create initialize Top 3 Heap (min heap) structure: **/
                std::array<Order, maxTopOrdersCount> top3Heap {};
                for (size_t i = 0; i < maxTopOrdersCount; ++i)
                    top3Heap[i] = buyOrders[i];

                for (size_t i = top3Heap.size(); i < buyOrders.size(); ++i) {
                    std::make_heap(top3Heap.begin(), top3Heap.end(), greaterByVolume);
                    if (greaterByVolume(buyOrders[i], top3Heap[0]))
                        top3Heap[0] = buyOrders[i];
                }
                std::for_each(top3Heap.cbegin(), top3Heap.cend(), [](const auto& o) { std::cout << "   " << o << std::endl;});
            } else {
                // TODO: Report/Log error maybe? Exception?
            }
        }

        /**
         * Finds the best SELL price and related order volume for a specific symbol at a given time.
         *
         * Prints out to the console (std::count) in free form the price and volume of one SELL
         * order with the maximum price out of all symbol orders with matching timestamp
         *
         * @param symbol - value of the order symbol (originator/client)
         * @param timestamp - timestamp without milliseconds as string, Example: ”15:30:00”
         * @return None
         */
        // INFO: According the description method shall ignore microseconds in input timestamp
        void BestSellAtTime(const std::string& symbol,
                            const std::string& timestamp) const  {
            if (const auto iter = book.find(symbol); book.end() != iter) {
                std::cout << "Best sell for '" << symbol << "' at " << timestamp << ":\n";
                const Orders::OrdersList &sellOrders = iter->second.sellOrders;
                if (sellOrders.empty()) {
                    std::cout << "   No sell orders found\n";
                    return;
                }

                Order bestSell {};
                bestSell.price = std::numeric_limits<double>::min();
                std::for_each(sellOrders.cbegin(), sellOrders.cend(), [&bestSell, &timestamp] (const auto& order) {
                    if (compareTimestampNoMsec(timestamp, order.timestamp) && order.price > bestSell.price) {
                        bestSell = order;
                    }
                });

                if (std::numeric_limits<double>::min() == bestSell.price)
                    std::cout << "   Couldn't find any sell orders created at " << timestamp << "\n";
                else
                    std::cout << "   [Price: " << bestSell.price << ", Volume: "  << bestSell.volume << "]\n";
            } else {
                // TODO: Report/Log error maybe? Exception?
            }
        }

    private:

        void parseOrders(const std::vector<std::string>& orders) {
            // TODO: Try use:std::array<std::string_view> ???
            for (const auto& rawOrder: orders) {
                const std::array<std::string, orderParts> order { splitOrder(rawOrder) };
                const OrderType type = getOrderType(order[OrderFields::Side]);

                Orders& clientOrders = book[order[OrderFields::Symbol]];
                auto& ordersByType = (OrderType::Buy == type) ? clientOrders.buyOrders : clientOrders.sellOrders;
                handleOrder(order, ordersByType);
            }
        }

        static void handleOrder(const std::array<std::string, orderParts>& orderParts,
                                Orders::OrdersList& orders ) {
            const Operation operation = getOperation(orderParts[OrderFields::Operation]);
            const auto idHash = hasher(orderParts[OrderFields::OrderID]);
            if (Operation::Insert == operation) {
                orders.emplace_back(orderParts[OrderFields::Timestamp],
                                    orderParts[OrderFields::OrderID],
                                    idHash,
                                    orderParts[OrderFields::Volume],
                                    orderParts[OrderFields::Price]);
            } else {
                if (auto iter = std::find_if(orders.begin(), orders.end(), [&](const auto &order) {
                        return idHash == order.idHash;
                    }); iter != orders.end()) {
                    if (Operation::Cancel == operation) {
                        orders.erase(iter);
                    } else {
                        iter->volume = str2Number(orderParts[OrderFields::Volume]);
                        iter->price = str2Number(orderParts[OrderFields::Price]);
                    }
                } else {
                    // TODO: Its an update operation but we failed to find the order
                    //       with given ID and Type ... throw exception maybe??
                }
            }
        }

        [[nodiscard]]
        static bool compareTimestampNoMsec(std::string_view time1,
                                           std::string_view time2) noexcept {
            const size_t pos1 = time1.find('.'), pos2 = time2.find('.');
            if (pos1 != pos2)
                return false;
            return time1.substr(0, pos1) == time2.substr(0, pos2);
        }

        // TODO: Check perf!!! String vs String_View
        // TODO: Run test against split() with std::vector<T>
        [[nodiscard]]
        static std::array<std::string, orderParts> splitOrder(std::string_view str) {
            std::array<std::string, orderParts> parts {};
            size_t pos = 0, prev = 0, idx = 0;
            while ((pos = str.find(';', prev)) != std::string::npos) {
                parts[idx++].assign(str, prev, pos - prev);
                prev = pos + 1;
            }
            parts[idx++].assign(str, prev, str.length() - prev);
            return parts;
        }
    };
};

namespace Tests {
    static constexpr std::string_view dataFilePath {
            R"(../data/orders1.dat)" };

    void BaseFunctionsTest() {
        OrderBookImpl::OrderBook orderBook{};
        if (!orderBook.readOrders(dataFilePath))
            return;

        orderBook.OrderCounts();
        std::cout << "\n\n";

        orderBook.BiggestBuyOrders("TEST8");
        std::cout << "\n\n";

        orderBook.BestSellAtTime("TEST8", "15:35:54.960008");
    }
}



int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    // Tests::BaseFunctionsTest();




    return EXIT_SUCCESS;
}

