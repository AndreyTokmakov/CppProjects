/**============================================================================
Name        : OrderBook.h
Created on  : 01.04.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : OrderBook
============================================================================**/

#include "OrderBook.h"

#include <iostream>
#include <charconv>
#include <string_view>
#include <vector>
#include <deque>
#include <ostream>
#include <list>
#include <array>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <chrono>

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

namespace {

    inline constexpr std::string_view dataFilePath {
            R"(/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/FinTechMarketProjects/data/orders1.dat)" };

    inline constexpr std::string_view dataFilePathTest {
            R"(/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/FinTechMarketProjects/data/orders_test1.dat)" };

    inline constexpr std::string_view dataFilePathD {
            R"(/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/FinTechMarketProjects/data/da_vinchi_orders.dat)"};


    inline constexpr size_t orderParts { 7 };

    inline constexpr std::string_view buyOrderString { "BUY" };
    inline constexpr std::string_view sellOrderString { "SELL" };
}

namespace OrderBookIml
{
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
        /** TODO: Remove with size_t **/
        std::string timestamp {};
        std::string id {};
        /** To improve look up speed. Will be used instead 'id' to match orders. **/
        size_t idHash {0};
        double volume {0};
        double price {0};

        Order() = default;

        // INFO: All strings in the example fits string SSO --> no need to move
        Order(const std::string& time,const std::string& id,
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
        // vector showed a little better performance in given test data regardless of the
        // significant amount of deletion of elements in the middle (not on the back) of the vector
        using OrdersList = std::vector<Order>;

        OrdersList buyOrders {};
        OrdersList sellOrders {};

        [[nodiscard]]
        size_t getOrdersTotal() const noexcept {
            return buyOrders.size() + sellOrders.size();
        }
    };


    class OrderBook {
    private:
        std::unordered_map<std::string, Orders> book {};

        /** OrderID hashing strategy: P.S. we should use DI here instead. **/
        constexpr static std::hash<std::string> hasher { std::hash<std::string>{} };

        /** Order's objects comparator: **/
        constexpr static auto greaterByVolume = [](const Order& x, const Order& y) {
            return x.volume > y.volume ;
        };

        constexpr static size_t maxTopOrdersCount { 3 };

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
        void readOrders(std::string_view path) {
            // TODO: Reserve?? Check lines count???
            // TODO: Check for performance
            std::vector<std::string> ordersData {};
            if (std::fstream file = std::fstream(path.data()); file.is_open() && file.good()) {
                while (std::getline(file, ordersData.emplace_back())) { /* */ }
            }
            if (ordersData.back().empty())
                ordersData.pop_back();
            parseOrders(ordersData);
        }


    public:
        void OrderCounts() const
        {
            size_t counter = 0;
            for (const auto& [symbol, orders]: book) {
                std::cout << symbol << " [Total: " << orders.getOrdersTotal()
                          << ", Buy: " << orders.buyOrders.size() << ", Sell: " << orders.sellOrders.size()
                          << "]\n";
                counter += orders.getOrdersTotal();
            }

            std::cout << "Size = " << counter << std::endl;
        }

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

                // TODO: Refactor???
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

        friend void PerformanceTests(OrderBook& orderBook, std::string_view path);
        friend void printInfo(OrderBook& orderBook);
    };
}

namespace OrderBookIml {
    //-----------------------------------------------------------------

    // TODO: Remove it!!!
    void PerformanceTests(OrderBook& orderBook, std::string_view path) {
        std::vector<std::string> ordersData {};
        if (std::fstream file = std::fstream(path.data()); file.is_open() && file.good()) {
            while (std::getline(file, ordersData.emplace_back())) { /* */ }
        }
        if (ordersData.back().empty())
            ordersData.pop_back();


        auto start = std::chrono::high_resolution_clock::now();

        constexpr int tests {500};
        for (int i = 0; i < tests; ++i) {
            orderBook.parseOrders(ordersData);
            //this->OrderCounts();
            orderBook.book.clear();
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "Result: " << duration << " microseconds" << std::endl;
    }

    void printInfo(OrderBook& orderBook)
    {
        for (const auto& [symbol, orders]: orderBook.book) {
            std::cout << symbol << " orders:\n";
            std::cout << "Buy:" << std::endl;
            std::for_each(orders.buyOrders.cbegin(), orders.buyOrders.cend(), [](const auto &o) {
                std::cout << "   " << o << std::endl;
            });
            std::cout << "Sell:" << std::endl;
            std::for_each(orders.sellOrders.cbegin(), orders.sellOrders.cend(), [](const auto &o) {
                std::cout << "   " << o << std::endl;
            });
        }
    }

    void Test() {
        OrderBook orderBook{};

        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        orderBook.readOrders(dataFilePathD);

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "It took me " << time_span.count() << " seconds.\n";



        // orderBook.OrderCounts();


        // orderBook.BiggestBuyOrders("TEST8"); std::cout << "\n\n";

        // orderBook.BestSellAtTime("TEST8", "15:35:54.960008");
        // orderBook.BestSellAtTime("TEST8", "15:35:54.960008");

    }

    void Perf() {
        OrderBook orderBook{};
        PerformanceTests(orderBook, dataFilePath);
    }
}

namespace Experiments {


    /*
    const auto [ptr, errCode] { std::from_chars(teamScoreStr.data() + pos + 1,
                                teamScoreStr.data() + teamScoreStr.size(), score) };
    if (errCode == std::errc()) {
        auto& team = teamsList.emplace_back();
        team.name.assign(teamScoreStr, 0, pos);
        team.goalsMean = score;
    }
    */

    // TODO: std::array<> ?? since we know the size& ??
    // TODO: Perf check string_view VS string
    void splitOrder(std::string_view str, std::vector<std::string>& cont) {
        size_t pos = 0, prev = 0;
        while ((pos = str.find(';', prev)) != std::string::npos) {
            cont.emplace_back(str, prev, pos - prev);
            prev = pos + 1;
        }
        cont.emplace_back(str, prev, str.length() - prev);
    }

    // TODO: Run test against split() with std::vector<T>
    void splitOrder(std::string_view str, std::array<std::string, orderParts>& cont) {
        size_t pos = 0, prev = 0, idx = 0;
        while ((pos = str.find(';', prev)) != std::string::npos) {
            cont[idx++].assign(str, prev, pos - prev);
            prev = pos + 1;
        }
        cont[idx++].assign(str, prev, str.length() - prev);
    }



    /** Comments!!!
     * Create a list of commands by reading it from a .csv file
     * @param path  - absolute path to file
     * @return  True is case of success and False otherwise
     */
    void ReadFile()
    {
        // TODO: Reserve?? Check lines count???
        // TODO: Check for performance
        std::vector<std::string> orders;
        if (std::fstream file = std::fstream (dataFilePath.data()); file.is_open() && file.good()) {
            while (std::getline(file, orders.emplace_back())) { /* */ }
        }
        orders.pop_back();


        // std::unordered_map<std::string, std::unordered_set<std::string>> clientsOrders{};
        std::unordered_map<std::string, int> clientsOrders{};
        for (const auto& rawOrder: orders)
        {
            std::array<std::string, orderParts> order{};
            splitOrder(rawOrder, order);


            auto& ids = clientsOrders[ order[1] ];
            if ("I" == order[3] )
                ids++;
            else if ("C" == order[3])
                ids--;

            // ids.insert(order[2]);

            //std::cout << order[0] << "  " << order[1] << "  " << order[2] << "  " << order[3] << "  "
             //             << order[4] << "  " << order[5] << "  " << order[6] << std::endl;
        }

        for (const auto& [k, v]: clientsOrders)
            std::cout << k << "  " << v << std::endl;
    }
}

void OrderBook::TestAll()
{
    /*
    std::vector<std::string> parts;
    splitOrder(order, parts);

    for (const auto& s: parts)
        std::cout << s << std::endl;
    */

    // Experiments::ReadFile();

    OrderBookIml::Test();
    // OrderBookIml::Perf();



}
