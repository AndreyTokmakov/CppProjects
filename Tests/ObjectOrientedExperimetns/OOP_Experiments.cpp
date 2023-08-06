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
#include <chrono>
#include "../Helpers/Utilities.h"

#include <cstring>

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

namespace ObjectOrientedExperiments::Clear_NonTrivial_Objects
{

    struct ARPHeader
    {
        uint16_t htype {0};
        uint16_t ptype {0};
        uint8_t  hlen {};
        uint8_t  plen {};
        uint16_t opcode {0};
        uint8_t  sender_mac[6]{};
        uint32_t sender_ip {};
        uint8_t  target_mac[6]{};
        uint32_t target_ip {};

        void info() {
            std::cout
            << htype << ','
            << ptype << ','
            << static_cast<uint16_t>(hlen) << ','
            << static_cast<uint16_t>(plen) << ','
            << opcode << ','
            << sender_mac << ','
            << sender_ip << ','
            << sender_ip << ','
            << target_ip << '\n';
        }

        void init() {
            htype = 1;
            ptype = 1;
            hlen = 1;
            plen = 1;
            opcode = 1;
            sender_ip = 1;
            target_ip = 1;
        }

    } __attribute__((packed, aligned(1)));

    void clear_memset(ARPHeader* arpHeader)
    {
        memset(arpHeader, 0, sizeof(ARPHeader));
    }

    void clear(ARPHeader* arpHeader)
    {
        *arpHeader = {};
    }

    void clearTest()
    {
        ARPHeader header;

        header.init();
        clear_memset(&header);
        header.info();

        header.init();
        clear(&header);
        header.info();
    }

#pragma optimize( "", off )
    void perfTest()
    {
        constexpr size_t testsCount = 100'000;

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

            ARPHeader header;
            for (size_t i = 0; i < testsCount; ++i) {
                for (size_t n = 0; n < testsCount; ++n) {
                    clear_memset(&header);
                }
            }
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

            ARPHeader header;
            for (size_t i = 0; i < testsCount; ++i) {
                for (size_t n = 0; n < testsCount; ++n) {
                    clear(&header);
                }
            }
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }
    }
#pragma optimize( "", on )
}


void ObjectOrientedExperiments::OOP_Experiments::TestAll()
{
    // OOP_Experiments::FriendTests

    // OOP_Test::ExampleTwo::test();  // overrided method call
    // OOP_Test::ExampleThree::test();   // static binding

    // OOP::MoveTest_CallMoveConstructor();

    // OOP::TestClassConversationOperatorCall();

    // Clear_NonTrivial_Objects::clearTest();
    // Clear_NonTrivial_Objects::perfTest();

    uint8_t  sender_mac[6]{};

    std::fill_n(sender_mac, 6, 0);
};
