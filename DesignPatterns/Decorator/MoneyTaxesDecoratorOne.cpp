/**============================================================================
Name        : MoneyTaxesDecoratorOne.cpp
Created on  : 12.03.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MoneyTaxesDecoratorOne src
============================================================================**/

#include "MoneyTaxesDecoratorOne.h"

#include <iostream>
#include <string_view>
#include <memory>

namespace MoneyTaxesDecoratorOne
{
    struct Money {
        uint64_t value{};
    };


    template<typename T> requires std::is_arithmetic_v<T>
    [[nodiscard]]
    Money operator*(const Money& money, T factor) {
        return Money {static_cast<uint64_t>( money.value * factor )};
    }

    [[nodiscard]]
    constexpr Money operator+(const Money& lhs, const Money& rhs) noexcept {
        return Money{lhs.value + rhs.value};
    }

    std::ostream &operator<<(std::ostream &stream, const Money &money) {
        stream << money.value;
        return stream;
    }

    class Item final {
    public:
        template<typename T>
        Item(T item): pimpl { std::make_unique<Model<T>>(std::move(item)) } {  /** **/ }

        Item(const Item &item) : pimpl {item.pimpl->clone() } {  /** **/ }

        Item &operator=(Item const &item) {
            pimpl = item.pimpl->clone();
            return *this;
        }

        ~Item() = default;

        Item(Item &&) = default;

        Item &operator=(Item &&item) = default;

        [[nodiscard]]
        Money price() const {
            return pimpl->price();
        }

    private:
        struct IConcept
        {
            [[nodiscard]]
            virtual Money price() const = 0;

            [[nodiscard]]
            virtual std::unique_ptr<IConcept> clone() const = 0;

            virtual ~IConcept() = default;
        };

        template<typename T>
        struct Model : public IConcept
        {
            explicit Model(T const &item) : item{item} {  /** **/ }

            explicit Model(T &&item) : item{std::move(item)} {}

            [[nodiscard]]
            Money price() const override {
                return item.price();
            }

            [[nodiscard]]
            std::unique_ptr<IConcept> clone() const override {
                return std::make_unique<Model<T>>(*this);
            }

            T item;
        };

        std::unique_ptr<IConcept> pimpl;
    };


    class Taxed
    {
    public:
        Taxed(double taxRate, Item item): item {std::move(item)}, factor { 1.0 + taxRate}
        {  /** **/ }

        [[nodiscard]]
        Money price() const {
            return item.price() * factor;
        }

    private:
        Item item;
        double factor;
    };


    class Discounted
    {
    public:
        Discounted(double discount, Item item): item {std::move(item)}, factor {1.0 - discount}
        {  /** **/ }

        [[nodiscard]]
        Money price() const {
            return item.price() * factor;
        }

    private:
        Item item;
        double factor;
    };

    struct Ticket
    {
        Ticket(std::string name, Money price ): name_{ std::move(name) } , price_{ price } {
            // ....
        }

        [[nodiscard]]
        const std::string& name() const {
            return name_;
        }

        [[nodiscard]]
        Money price() const {
            return price_;
        }

    private:
        std::string name_;
        Money price_;
    };


    struct Book
    {
        Book(std::string name, Money price ): name_{ std::move(name) }, price_{ price }
        {}

        [[nodiscard]]
        std::string const& name() const {
            return name_;
        }

        [[nodiscard]] Money price() const {
            return price_;
        }

    private:
        std::string name_;
        Money price_;
    };

}


void MoneyTaxesDecoratorOne::Test()
{
    Item item1(Taxed( 0.15,Discounted(0.2, Ticket{ "Core C++", Money{499}})));
    Item item2(Taxed( 0.17,Discounted(0.2, Ticket{ "Core C++", Money{499}})));
    Item item3(Taxed( 0.18,Discounted(0.21, Book{ "Software Design", Money{499}})));

    const Money price1 = item1.price();
    const Money price2 = item2.price();
    const Money price3 = item3.price();

    std::cout << price1 << std::endl;
    std::cout << price2 << std::endl;
    std::cout << price3 << std::endl;
}

