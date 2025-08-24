/**============================================================================
Name        : CallbackTests.cpp
Created on  : 24.07.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CallbackTests.cpp
============================================================================**/

#include "CallbackTests.hpp"

#include <iostream>
#include <string_view>
#include <functional>
#include <numeric>
#include <utility>

namespace CallbackTests
{
    struct Consumer
    {
        void accept(const std::string& message) const
        {
            std::cout << "Consumer: " << message << std::endl;
        }
    };

    using MessageListener = std::function<void(Consumer& consumer, const std::string& msg)>;


    struct Manager
    {
        MessageListener callback;

        void setMessageListener(MessageListener messageListener)
        {
            callback = std::move(messageListener);
        }

        void pushMessage(Consumer& consumer, const std::string& str) const{
            callback(consumer, str);
        }

        void callbackMemberFunction(const Consumer& consumer, const std::string& str)
        {
            consumer.accept("[" + str + "]");
            consumer.accept("{" + str + "}");
            consumer.accept("(" + str + ")");
        }

        void init()
        {
            /*
            setMessageListener([](Consumer& consumer, const std::string& str){
                consumer.accept("[" + str + "]");
            });
            */

            setMessageListener([&](Consumer& consumer, const std::string& str){
                callbackMemberFunction(consumer, str);
            });
        }
    };


    void Tests()
    {
        Manager manager;

        manager.init();

        /*
        manager.setMessageListener([](Consumer& consumer, const std::string& str){
            consumer.accept("[" + str + "]");
        });*/

        Consumer consumer;
        manager.pushMessage(consumer, "One");
    }
}


namespace CallbackTestsRef
{
    struct Consumer
    {
        void accept(const std::string& message) const
        {
            std::cout << "Consumer: " << message << std::endl;
        }
    };

    using MessageListener = std::function<void(Consumer& consumer, const std::string& msg)>;


    struct Manager
    {
        MessageListener& callback;

        explicit Manager(MessageListener messageListener):
                callback {messageListener} {}

        void pushMessage(Consumer& consumer, const std::string& str) const{
            callback(consumer, str);
        }
    };

    void Tests()
    {
        Manager manager([](Consumer& consumer, const std::string& str){
            consumer.accept("[" + str + "]");
        });

        Consumer consumer;
        manager.pushMessage(consumer, "One");
    }
}

namespace CallbackTests::PassClassMethodAsCallback
{

    bool isEvenGlobal(const int x) {
        return ! (x & 1);
    }

    struct Worker
    {
        std::vector<int> values;;

        Worker(): values(10)
        {
            std::iota((values.begin()), values.end(), 0);
        }

        template<std::predicate<int> Func>
        void printFiltered(Func func) const noexcept
        {
            for (int a : values) {
                if (func(a)) {
                    std::cout << a << " ";
                }
            }
            std::cout << std::endl;
        }

        bool isEven (const int x) {
            return ! (x & 1);
        }


        void test() const
        {
            auto isEvenLambda = [](const int x)-> int {
                return ! (x & 1);
            };

            printFiltered(isEvenLambda);
            printFiltered(isEvenGlobal);
        }
    };


    void demo()
    {
        const Worker worker;
        worker.test();
    }
}


void CallbackTests::TestAll()
{
    // CallbackTests::Tests();

    // TODO: Here we have a crush :
    //       Because the CallbackTestsRef::MessageListener& callback  --> is Reference type variable
    //    Linter:
    //       Reference member declared here CallbackTests. cpp:100:27:
    //       warning: binding reference member 'callback' to stack allocated parameter 'messageListener

    // CallbackTestsRef::Tests();

    PassClassMethodAsCallback::demo();

}