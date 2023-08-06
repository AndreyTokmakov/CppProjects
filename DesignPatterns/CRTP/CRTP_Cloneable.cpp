/**============================================================================
Name        : CRTP_Cloneable.cpp
Created on  : 06.08.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CRTP Cloneable demo
============================================================================**/

#include "CRTP_Cloneable.h"

#include <iostream>
#include <string_view>
#include <memory>

namespace CRTP::CRTP_Cloneable
{
    class IBase
    {
    public:
        // Destructor always virtual to avoid memory leak,
        virtual ~IBase() = default;

        // "virtual copy constructor"
        [[nodiscard]]
        virtual std::unique_ptr<IBase> clone() const = 0;

        // "virtual default constructor"
        [[nodiscard]]
        virtual std::unique_ptr<IBase> create() const = 0;

        [[nodiscard]]
        virtual std::string getID() const = 0;

        // virtual void setID(std::string id) = 0;  // INFO: No need to demo

        virtual void show() const = 0;
    };

    template<typename Base, typename Derived>
    class Copyable : public Base {
    private:
        [[nodiscard]]
        inline const Derived &self() const noexcept {
            return *static_cast<const Derived *const>(this);
        }

    public:
        virtual ~Copyable() = default;

        [[nodiscard]]
        std::unique_ptr<Base> clone() const override {
            std::cout << "Cloning " << typeid(Derived).name() << std::endl;
            return std::make_unique<Derived>(this->self());
        }

        [[nodiscard]]
        std::unique_ptr<Base> create() const override {
            return std::make_unique<Derived>();
        }
    };

    class DerivedA : public Copyable<IBase, DerivedA> {
    private:
        std::string id;
    public:
        explicit DerivedA() : id("unnamed-A") {}

        explicit DerivedA(std::string id) : id { std::move(id) } {}

        [[nodiscard]]
        std::string getID() const override { return id; }
        // void setID(std::string id) override { this->id = id; } // INFO: No need to demo

        void show() const override {
            std::cout << " => Class DerivedA - id = " << id << "\n";
        }
    };


    class DerivedB : public Copyable<IBase, DerivedB> {
    private:
        std::string id;
    public:
        DerivedB() : id("unnamed-B") {}

        explicit DerivedB(std::string id) : id { std::move(id) } {}

        [[nodiscard]]
        std::string getID() const override { return id; }
        // void setID(std::string id) override { this->id = id; } // INFO: No need to demo

        void show() const override {
            std::cout << " => Class DerivedB - id = " << id << "\n";
        }
    };
}

void CRTP::CRTP_Cloneable::Test()
{
    DerivedA a("__AAAAA__");
    DerivedB b("__BBBBB__");

    a.show();
    b.show();

    std::cout << "----------------------------------------------------------\n";

    IBase *aPtr = &a, *bPtr = &b;
    aPtr->show();
    bPtr->show();

    std::cout << "----------------------------------------------------------\n";

    std::unique_ptr<IBase> clone = aPtr->clone();
    clone->show();

    clone = bPtr->clone();
    clone->show();
}
