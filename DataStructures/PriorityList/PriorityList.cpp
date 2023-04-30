//============================================================================
// Name        : PriorityList.cpp
// Created on  : 07.05.22.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : PriorityList
//============================================================================

#include "PriorityList.h"

#include <iostream>
#include <algorithm>
#include <list>

namespace PriorityList {

    /*
    struct Object final {
        size_t id{0};
        size_t volume{0};
        double price{0};
        // Timestamp:

        // INFO: Remove CTors??
        Object() = default;

        Object(size_t id, size_t volume, double price) :
                id{id}, volume{volume}, price{price} {
        }

        bool operator==(const Object &order) const noexcept {
            return this->id == order.id && this->volume == order.volume && this->price == order.price;
        }

        bool operator<(const Object &order) const noexcept {
            return this->price < order.price;
        }
    };

    std::ostream &operator<<(std::ostream &stream, const Object &order) {
        stream << "Order [id: " << order.id << ", price: " << order.price << ", volume: " << order.volume;
        return stream;
    }
    */

    template<typename T>
    class OrderList : public std::list<T> {
    public:
        void add(T value) {
            const auto iter = std::find_if(this->cbegin(), this->cend(), [&](const auto &v) {
                return value >= v;
            });
            this->insert(iter, value);
        }

    private:
        using std::list<T>::push_back;
        using std::list<T>::push_front;
    };

}

void PriorityList::TEST_ALL() {

    OrderList<int> orders {};

    orders.add(13);
    orders.add(2);
    orders.add(33);
    orders.add(33);


    for (const auto& v: orders)
        std::cout << v << std::endl;
}