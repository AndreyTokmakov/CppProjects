/**============================================================================
Name        : OOP_Experiments.cpp
Created on  : 16.07.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : OOP_Experiments
============================================================================**/

#include "OOP_Experiments.h"

#include <iostream>
#include <memory>
#include <vector>
#include <span>
#include "../Helpers/Utilities.h"

namespace OOP_Experiments::FriendTests
{
    class A {
        int v {0};

        friend class B;
    };

    class B {
    public:
        void foo(A& a) {
            a.v = 10;
        }
    };
};


namespace OOP
{
    void MoveTest_CallMoveConstructor()
    {
        Helpers::Long l1 {111};

        std::cout << "------------------------------------------------\n";

        Helpers::Long l2 = std::move(l1);
    }

    struct Base {
        Base() {
            std::cout << "Base::Base()" << std::endl;
        }

        operator short() const {
            std::cout << "operator Base::short()" << std::endl;
            return 1;
        }
    };

    struct DerivedClass: Base {
        DerivedClass() {
            std::cout << "Derived::Derived()" << std::endl;
        }
    };

    void count(int) {

    }

    void TestClassConversationOperatorCall()
    {
        const DerivedClass object;
        count(object); // Will not compile if 'operator short() const ' --> explicit
    }
}


namespace OOP_Test
{
    struct Element {
        std::string name {};
    };

    std::vector<Element> getElements() {
        return {
                Element{"ElementOne"},
                Element{"ElementTwo"},
                Element{"ElementThree"},
                Element{"ElementFour"},
                Element{"ElementFive"}
        };
    }

    namespace ExampleTwo
    {
        struct HandlerBase
        {
            void handle(std::span<Element> elements) {
                for (Element el: elements)
                    handle(std::move(el));
            }

            virtual void handle(Element element) = 0;
            virtual ~HandlerBase() = default;
        };

        struct HandlerA: HandlerBase {
            void handle(Element element) override {
                std::cout << "HandlerA: " << element.name << std::endl;
            }
        };

        struct HandlerB: HandlerBase {
            void handle(Element element) override {
                std::cout << "HandlerA: " << element.name << std::endl;
            }
        };

        std::unique_ptr<HandlerBase> getHandler()
        {
            return std::make_unique<HandlerA>();
        }

        void test()
        {
            std::unique_ptr<HandlerBase> handler = getHandler();
            std::vector<Element> elements = getElements();
            handler->handle(elements);
        }
    }

    namespace ExampleThree
    {
        struct HandlerBase
        {
            virtual void handle(std::span<Element> elements) = 0;
            virtual ~HandlerBase() = default;
        };

        template<class Derived>
        struct Handler: HandlerBase {
            void handle(std::span<Element> elements) {
                for (Element el: elements)
                    d.handle(std::move(el));
            }

            Derived d {};
        };

        struct HandlerA {
            void handle(const Element& element) {
                std::cout << "HandlerA: " << element.name << std::endl;
            }
        };

        struct HandlerB {
            void handle(const Element& element) {
                std::cout << "HandlerA: " << element.name << std::endl;
            }
        };

        std::unique_ptr<HandlerBase> getHandler()
        {
            return std::make_unique<Handler<HandlerA>>();
        }

        void test()
        {
            std::unique_ptr<HandlerBase> handler = getHandler();
            std::vector<Element> elements = getElements();
            handler->handle(elements);
        }
    }
}



void ObjectOrientedExperiments::OOP_Experiments::TestAll()
{
    // OOP_Experiments::FriendTests

    // OOP_Test::ExampleTwo::test();  // overrided method call
    // OOP_Test::ExampleThree::test();   // static binding

    OOP::MoveTest_CallMoveConstructor();

    OOP::TestClassConversationOperatorCall();
};
