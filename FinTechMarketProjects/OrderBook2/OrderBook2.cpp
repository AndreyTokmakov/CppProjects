/**============================================================================
Name        : OrderBook2.cpp
Created on  : 01.04.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : OrderBook2
============================================================================**/


#include "OrderBook2.h"

#include <iostream>
#include <charconv>
#include <string_view>
#include <vector>
#include <deque>
#include <ostream>
#include <list>
#include <set>
#include <array>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <chrono>

namespace OrderBook2
{
    inline constexpr std::string_view buyOrderString { "BUY" };
    // inline constexpr std::string_view sellOrderString { "SELL" };

    inline constexpr std::string_view insertOrderParam { "Insert" };
    // inline constexpr std::string_view cancelOrderParam { "Cancel" };
    inline constexpr std::string_view amendOrderParam  { "Amend" };


    enum class OrderSide {
        Buy,
        Sell
    };

    enum class Operation {
        Insert,
        Cancel,
        Amend,
    };

    [[nodiscard]]
    OrderSide getOrderType(std::string_view method) {
        return buyOrderString == method ? OrderSide::Buy : OrderSide::Sell;
    }

    [[nodiscard]]
    Operation getOperation(std::string_view operation) {
        if (insertOrderParam == operation)
            return Operation::Insert;
        else if (amendOrderParam == operation)
            return Operation::Amend;
        return Operation::Cancel;
    }

    [[nodiscard]]
    double str2Number(const std::string& str) {
        return std::strtod(str.data(), nullptr);
    }
}


namespace {
    constexpr std::string_view dataFilePath {
        R"(/home/andtokm/DiskS/Temp/BOOK_ORDER_DATA/BTC-PERPETUAL.csv)" };
}



struct Order final {
    size_t id {0};
    size_t volume {0};
    double price {0};
    // Timestamp:

    // INFO: Remove CTors??
    Order() = default;
    Order(size_t id, size_t volume, double price):
            id {id},  volume {volume}, price {price} {
    }

    bool operator==(const Order& order) const noexcept {
        return this->id == order.id && this->volume == order.volume && this->price == order.price;
    }

    bool operator<(const Order& order) const noexcept {
        return this->price < order.price;
    }
};

std::ostream& operator<<(std::ostream& stream, const Order& order) {
    stream << "Order [id: " << order.id << ", price: " << order.price << ", volume: " << order.volume;
    return stream;
}


template<typename T>
class OrderList: public std::list<T> {
public:
    void add(T value) {
        const auto iter = std::find_if(this->cbegin(), this->cend(), [&](const auto& v) {
            return value >= v;
        });
        this->insert(iter, value);
    }

private:
    using std::list<T>::push_back;
    using std::list<T>::push_front;
};

void OrderBook2::TestAll()
{
    /*
    int counter = 0;
    std::vector<std::string> orders {};
    if (std::fstream file = std::fstream (dataFilePath.data()); file.is_open() && file.good()) {
        while (std::getline(file, orders.emplace_back())) {
            if (counter++ > 10) break;
        }
    }

    for (const std::string& str: orders)
        std::cout << str << std::endl;

    std::cout << "OrderBook2 tests\n";
     */





    /*
    OrderList<int> orders {};

    orders.add(13);
    orders.add(2);
    orders.add(33);
    orders.add(33);


    // orders.push_back(1);

    for (const auto& v: orders)
        std::cout << v << std::endl;
    */

    std::set<Order> orders {};

    orders.emplace(1, 10, 4.5);
    orders.emplace(2, 4, 3.5);
    orders.emplace(3, 41, 1.5);

    for (const auto& v: orders)
        std::cout << v << std::endl;
}
