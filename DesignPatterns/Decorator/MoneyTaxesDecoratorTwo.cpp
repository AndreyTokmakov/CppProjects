/**============================================================================
Name        : MoneyTaxesDecoratorTwo.cpp
Created on  : 12.03.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MoneyTaxesDecoratorTwo src
============================================================================**/

#include "MoneyTaxesDecoratorTwo.h"

#include <iostream>
#include <string_view>
#include <memory>

namespace MoneyTaxesDecoratorTwo
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


    template<typename T>
    concept PricedItem = requires(T item) {
        { item.price() } -> std::same_as<Money>;
    };

    template<int taxRate, PricedItem Item>
    class Taxed : private Item {
    public:
        template<typename... Args>
        explicit Taxed(Args&& ... args): Item {std::forward<Args>(args)...}  {  /** **/ }

        [[nodiscard]]
        Money price() const {
            return Item::price() * (1.0 + (taxRate / 100));
        }
    };

    template<int discount, PricedItem Item>
    class Discounted {
    public:
        template<typename... Args>
        explicit Discounted(Args&& ... args): item{std::forward<Args>(args)...} {
            // ....
        }

        [[nodiscard]]
        Money price() const {
            return item.price() * (1.0 - (discount / 100));
        }

    private:
        Item item;
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


void MoneyTaxesDecoratorTwo::Test()
{
    Taxed<15, Discounted<20, Ticket>> item1 { "Core C++", Money{499} };
    Taxed<16, Discounted<21, Ticket>> item2 { "Core C++", Money{499} };
    Taxed<17, Discounted<22, Book>> item3 { "Core C++", Money{499} };

    const Money price1 = item1.price();
    const Money price2 = item2.price();
    const Money price3 = item3.price();

    std::cout << price1 << std::endl;
    std::cout << price2 << std::endl;
    std::cout << price3 << std::endl;
}

