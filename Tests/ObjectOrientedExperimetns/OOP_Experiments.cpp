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
#include "../Helpers/Helpers.h"

#include <cstring>

using Integer = Helpers::Integer;
using Long = Helpers::Long;

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
        Helpers::Integer l1 {111};

        std::cout << "------------------------------------------------\n";

        Helpers::Integer l2 = std::move(l1);
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


namespace ObjectOrientedExperiments::CopyObjects
{

    struct SocketScoped
    {
        static constexpr int INVALID_HANDLE { -1 };
        static constexpr int SOCKET_ERROR { -1 };

        int handle { INVALID_HANDLE };

        SocketScoped(int s = INVALID_HANDLE) : handle { s } {
        }

        SocketScoped& operator=(int s) {
            handle = s;
            return *this;
        }

        SocketScoped(const SocketScoped& sock) = default;
        SocketScoped& operator=(const SocketScoped& sock) = default;

        SocketScoped(SocketScoped&& sock) noexcept :
                handle { std::exchange(sock.handle, INVALID_HANDLE)}  {
        }

        SocketScoped& operator=(SocketScoped&& sock) noexcept {
            handle = std::exchange(sock.handle, INVALID_HANDLE);
            return *this;
        }

        operator int() const { // No explicit
            return handle;
        }

        [[nodiscard]]
        inline bool isValid() const noexcept {
            return INVALID_HANDLE == handle;
        }

        explicit operator bool() const noexcept {
            return (INVALID_HANDLE != handle);
        }

        ~SocketScoped() {
            closeSocket(handle);
        }

    private:
        static void closeSocket(int s) {
            std::cout << "Close socket (" << s << ")\n";
        }
    };

    SocketScoped createSocket()
    {
        SocketScoped socket = 123;
        if (-1 == socket) {
            std::cerr << "Failed to create socket. Error = " << errno << std::endl;
        }
        return socket;
    }

    void createSocketTest()
    {
        SocketScoped s = createSocket();
    }
}

namespace ObjectOrientedExperiments::DefaultConstructor_VariablesInitialization
{
    struct Keeper
    {
        Integer first {1};
        Integer second {2};

        Keeper() = default;
        explicit Keeper(int a): first {a}, second {200} {
        }

        /*
        Keeper(int a, int b): Keeper(), first{1} {
        }
        */
    };

    void Test()
    {
        {
            Keeper obj{};
        }

        std::cout << std::endl;

        {
            Keeper obj { 100};
        }

        /*
        std::cout << std::endl;

        {
            Keeper obj { 100, 200};
        }
        */
    }
}

namespace Change_Overrided_Method_Visibility
{

    struct Base
    {
        virtual void info() {
            std::cout << "Public Base::Info()\n";
        }

        virtual ~Base() = default;
    };

    struct Derived: Base
    {
    private:
        void info() override {
            std::cout << "Private Derived::Info()\n";
        }
    };

    void Test()
    {
        std::unique_ptr<Base> obj{std::make_unique<Derived>()};
        obj->info();
    }
}

namespace ObjectOrientedExperiments::ShadowingMemberVariable
{
    struct Base
    {
        int a {0};

        void printA() const noexcept {
            std::cout << a << std::endl;
        }
    };

    struct Derived: Base
    {
        int a {0};

        void setA(int v) noexcept {
            a = v;
        }
    };

    void Test()
    {
        Derived d;

        d.printA();
        std::cout << d.a << std::endl;

        d.setA(123);
        std::cout << std::endl;

        d.printA();
        std::cout << d.a << std::endl;
    }
}



void ObjectOrientedExperiments::OOP_Experiments::TestAll()
{
    // OOP_Experiments::FriendTests

    // OOP_Test::ExampleTwo::test();     // overrided method call
    // OOP_Test::ExampleThree::test();   // static binding

    // OOP::MoveTest_CallMoveConstructor();

    // OOP::TestClassConversationOperatorCall();

    // Clear_NonTrivial_Objects::clearTest();
    // Clear_NonTrivial_Objects::perfTest();

    // CopyObjects::createSocket();

    // DefaultConstructor_VariablesInitialization::Test();

    // Change_Overrided_Method_Visibility::Test();

    // ShadowingMemberVariable::Test();



    struct Base {
        virtual ~Base() = default;
    };

    struct Derived: Base {
        ~Derived() override {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }
    };

    std::unique_ptr<Base> ptr { std::make_unique<Derived>()};


};
