

#include <optional>
#include <iostream>
#include <string>
#include <string_view>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <charconv>
#include <ostream>

#include <vector>
#include <any>
#include <list>
#include <deque>
#include <map>
#include <algorithm>
#include <array>
#include <unordered_map>


#include "Experiments.h"

namespace {

    constexpr size_t orderParts { 10 };

    [[nodiscard]]
    std::array<std::string, orderParts> splitOrder(std::string_view str) {
        std::array<std::string, orderParts> parts {};
        size_t pos = 0, prev = 0, idx = 0;
        while ((pos = str.find(',', prev)) != std::string::npos) {
            parts[idx++].assign(str, prev, pos - prev);
            prev = pos + 1;
        }
        parts[idx++].assign(str, prev, str.length() - prev);
        return parts;
    }
}

namespace Experiments::Utils {

    struct StopWatch final {
        std::chrono::high_resolution_clock::time_point start { std::chrono::high_resolution_clock::now() };

        /*
        ~StopWatch() {
            const std::chrono::high_resolution_clock::time_point end { std::chrono::high_resolution_clock::now() };
            const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "Time: " << time_span.count() << std::endl;

            start = std::chrono::high_resolution_clock::now();
        }
        */

        void printTime() {
            const std::chrono::high_resolution_clock::time_point end { std::chrono::high_resolution_clock::now() };
            const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "Time: " << time_span.count() << std::endl;

            start = std::chrono::high_resolution_clock::now();
        }
    };
}

namespace Experiments::Tests
{
    struct RawOrderIDs final {
        static constexpr size_t SYMBOL {0};
        static constexpr size_t SYSTEM {1};
        static constexpr size_t TYPE {2};
        static constexpr size_t MOMENT {3};
        static constexpr size_t ID {4};
        static constexpr size_t ACTION {5};
        static constexpr size_t PRICE {6};
        static constexpr size_t VOLUME {7};
        static constexpr size_t ID_DEAL {8};
        static constexpr size_t PRICE_DEAL {9};
    };

    void AnalyzeData(std::string_view path)
    {
        std::vector<std::string> ordersRaw {};
        if (std::fstream file = std::fstream(path.data()); file.is_open() && file.good()) {
            while (std::getline(file, ordersRaw.emplace_back())) { /* */ }
        }
        ordersRaw.pop_back();

        const std::vector<std::array<std::string, orderParts>> ordersParsed = [&] {
            std::vector<std::array<std::string, orderParts>> ordersParsed {};
            ordersParsed.reserve(ordersRaw.size());
            for (size_t idx = 1; idx < ordersRaw.size(); ++idx)
                ordersParsed.push_back(splitOrder(ordersRaw[idx]));
            return ordersParsed;
        } ();
        ordersRaw.clear();

        const std::string& client = ordersParsed.front()[RawOrderIDs::SYMBOL];

        for (const auto& order: ordersParsed) {
            if (client == order[RawOrderIDs::SYMBOL]) {
                std::cout << order[RawOrderIDs::SYMBOL] << "  "
                          << order[RawOrderIDs::ID] << "  "
                          << order[RawOrderIDs::ACTION] << "  "
                          << order[RawOrderIDs::TYPE] << "  "
                          << order[RawOrderIDs::PRICE] << "  "
                          << order[RawOrderIDs::VOLUME] << "  "
                          << std::endl;
                if (order[RawOrderIDs::ACTION].find("2") != std::string::npos) {
                    std::cout << "*************************** DONE ******************************\n";
                }
            }
        }

        /*
        std::unordered_set<std::string> ids {}, types {}, systems {}, actions {};
        for (const auto& order: ordersParsed) {
            ids.insert(order[RawOrderIDs::SYMBOL]);
            systems.insert(order[RawOrderIDs::SYSTEM]);
            types.insert(order[RawOrderIDs::TYPE]);
            actions.insert(order[RawOrderIDs::ACTION]);
        }

        std::cout << "-------------------- entry --------------------\n";
        for (const auto& entry: systems)
            std::cout << entry << std::endl;

        std::cout << "-------------------- types --------------------\n";
        for (const auto& entry: types)
            std::cout << entry << std::endl;

        std::cout << "-------------------- actions --------------------\n";
        for (const auto& entry: actions)
            std::cout << entry << std::endl;
        */
    }
}

namespace Experiments::ParseTests {

    constexpr std::string_view text { "GZM9,F,S,20181228185033900,33948174935,0,14092.00201,2,," };

    enum class System: unsigned char {
        F = 'F',
    };

    enum class Type: unsigned char {
        Sell = 'S',
        Buy = 'B'
    };

    enum class Action: unsigned char {
        Add = '0',
        Modify = '1',
        Delete = '2'
    };

    struct Order {
        std::string symbol {};
        System system {};
        Type type {};
        std::string date {};
        std::string id {};
        Action action {0};
        double price {0.0};
        uint32_t volume {0};
    };

    Order ParseLine(std::string_view str) {

        constexpr char delim { ',' };
        size_t start = 0, end = 0;
        Order order {};

        if (std::string::npos != (end = str.find(delim, start + 1))) {
            order.symbol.assign(str.substr(start, end));
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            order.system = static_cast<System>(str.substr(start, end - start).front());
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            order.type = static_cast<Type>(str.substr(start, end - start).front());
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            order.date.assign(str.substr(start, end - start));
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            order.id.assign(str.substr(start, end - start));
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            order.action = static_cast<Action>(str.substr(start, end - start).front());
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            std::from_chars(str.data() + start, str.data() + end, order.price);
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            std::from_chars(str.data() + start, str.data() + end, order.volume);
        }

        /*
        std::cout.precision(32);
        std::cout << order.symbol<< std::endl;
        if (order.system == System::F)
            std::cout << "System::F" << std::endl;

        if (order.type == Type::Buy)
            std::cout << "Type::Buy" << std::endl;
        else if (order.type == Type::Sell)
            std::cout << "Type::Sell" << std::endl;

        std::cout << order.date << std::endl;
        std::cout << order.id << std::endl;

        if (order.action == Action::Add)
            std::cout << "Action::Add" << std::endl;
        else if (order.action == Action::Modify)
            std::cout << "Action::Modify" << std::endl;
        else if (order.action == Action::Delete)
            std::cout << "Action::Delete" << std::endl;

        std::cout << order.price << std::endl;
        std::cout << order.volume << std::endl;*/

        return order;
    }

    Order ParseLine2(const std::string& str) {
        const auto params = splitOrder(str);
        Order order {};

        order.symbol.assign(params[0]);
        order.system = static_cast<System>(params[1].front());
        order.type = static_cast<Type>(params[2].front());
        order.date.assign(params[3]);
        order.id.assign(params[4]);
        order.action = static_cast<Action>(params[5].front());

        const auto& s = params[6];
        std::from_chars(s.data(), s.data() + s.length(), order.price);

        const auto& s1 = params[7];
        std::from_chars(s1.data(), s1.data() + s1.length(), order.volume);

        /*
        std::cout.precision(32);
        std::cout << order.symbol<< std::endl;
        if (order.system == System::F)
            std::cout << "System::F" << std::endl;

        if (order.type == Type::Buy)
            std::cout << "Type::Buy" << std::endl;
        else if (order.type == Type::Sell)
            std::cout << "Type::Sell" << std::endl;

        std::cout << order.date << std::endl;
        std::cout << order.id << std::endl;

        if (order.action == Action::Add)
            std::cout << "Action::Add" << std::endl;
        else if (order.action == Action::Modify)
            std::cout << "Action::Modify" << std::endl;
        else if (order.action == Action::Delete)
            std::cout << "Action::Delete" << std::endl;

        std::cout << order.price << std::endl;
        std::cout << order.volume << std::endl<< std::endl;*/

        return order;
    }

    void ReadAndParse(std::string_view path) {
        Utils::StopWatch watch;
        std::string line {};
        if (std::fstream file = std::fstream(path.data()); file.is_open() && file.good()) {
            while (std::getline(file, line)) { /* */
                ParseLine(line);
            }
        }
        watch.printTime();
    }

    void ReadAndParse2(std::string_view path) {
        Utils::StopWatch watch;
        /*
        std::vector<std::string> ordersRaw {};
        if (std::fstream file = std::fstream(path.data()); file.is_open() && file.good()) {
            while (std::getline(file, ordersRaw.emplace_back())) { }
        }
        ordersRaw.pop_back();

        for (size_t idx = 1; idx < ordersRaw.size(); ++idx)
            ParseLine2(ordersRaw[idx]);
        */

        std::string line {};
        if (std::fstream file = std::fstream(path.data()); file.is_open() && file.good()) {
            while (std::getline(file, line)) {
                ParseLine2(line);
            }
        }
        watch.printTime();
    }

    void ParseSingleLineTest() {
        ParseLine(text);
    }
}


namespace Experiments::DataTypes {

    enum class System: unsigned char {
        F = 'F',
    };

    enum class Type: unsigned char {
        Sell = 'S',
        Buy = 'B'
    };

    enum class Action: unsigned char {
        Add = '0',
        Modify = '1',
        Delete = '2'
    };

    struct Order {
        std::string symbol {};
        System system {};
        Type type {};
        std::string date {};
        std::string id {};
        Action action {0};
        double price {0.0};
        uint32_t volume {0};
    };

}

namespace Experiments::Reader
{
    constexpr char delim { ',' };
    using DataTypes::System,
            DataTypes::Type,
            DataTypes::Action,
            DataTypes::Order;

    [[nodiscard("Do not ignore me")]]
    Order parseRawOrder(std::string_view str) {
        size_t start = 0, end = 0;
        Order order {};

        if (std::string::npos != (end = str.find(delim, start + 1))) {
            order.symbol.assign(str.substr(start, end));
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            order.system = static_cast<System>(str.substr(start, end - start).front());
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            order.type = static_cast<Type>(str.substr(start, end - start).front());
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            order.date.assign(str.substr(start, end - start));
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            order.id.assign(str.substr(start, end - start));
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            order.action = static_cast<Action>(str.substr(start, end - start).front());
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            std::from_chars(str.data() + start, str.data() + end, order.price);
        }

        if (std::string::npos != (start = str.find(delim, end)) &&
            std::string::npos != (end = str.find(delim, ++start))) {
            std::from_chars(str.data() + start, str.data() + end, order.volume);
        }
        return order;
    }

    [[nodiscard]]
    std::vector<Order> readOrders(std::string_view path) {
        std::vector<Order> orders {};
        orders.reserve(10'000'000); // FIXME: Hack
        std::string line {};
        if (std::fstream file = std::fstream(path.data()); file.is_open() && file.good()) {
            while (std::getline(file, line)) { /* */
                // parseRawOrder(line);
                orders.push_back(parseRawOrder(line));
            }
        }
        return orders;
    }
}


namespace Experiments::OrderBook
{
    using DataTypes::System, DataTypes::Type, DataTypes::Action, DataTypes::Order;

    struct ClientOrders final {
        std::map<double, Order> sell {};  // sort increase
        std::map<double, Order> buy  {};  // sort decrease
    };


    void handleOrders(const std::vector<Order>& orders) {
        std::unordered_map<std::string, ClientOrders> orderBook {};

        for (const Order& order: orders) {
            auto [iter, inserted] = orderBook.emplace(order.symbol, ClientOrders{});
            std::map<double, Order>& symbolOrders = (Type::Sell == order.type) ? iter->second.sell : iter->second.buy;

            symbolOrders.emplace(order.price, order);
        }

        std::cout << orderBook.size() << std::endl;
    }
}

void Experiments::TestAll() {

    static constexpr std::string_view dataFilePath {
            R"(/home/andtokm/DiskS/Temp/BOOK_ORDER_DATA/OrderLog_All/20181229_fut_ord.csv)" };

    // Tests::ReadData(dataFilePath);
    // Tests::AnalyzeData(dataFilePath);

    ParseTests::ReadAndParse(dataFilePath);
    // ParseTests::ReadAndParse2(dataFilePath);
    // ParseTests::ParseSingleLineTest();

    /*
    Utils::StopWatch watch {};

    const std::vector<DataTypes::Order> orders = Reader::readOrders(dataFilePath);

    watch.printTime();

    OrderBook::handleOrders(orders);
    */
}