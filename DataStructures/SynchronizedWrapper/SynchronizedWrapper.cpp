/**============================================================================
Name        : SynchronizedWrapper.cpp
Created on  : 19.04.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SynchronizedWrapper.cpp
============================================================================**/

#include "SynchronizedWrapper.hpp"

#include <print>
#include <string>
#include <mutex>

namespace
{
    struct Resource
    {
        explicit Resource(const int value = 0): v { value }{
        }

        Resource(const Resource&) = delete;
        Resource& operator=(const Resource&) = delete;

        int increment() {
            return ++v;
        }

        [[nodiscard]]
        int getValue() const noexcept {
            return v;
        }

    private:

        int v { 0 };
    };




    template<typename Ty>
    concept Lockable = requires(Ty& lk)
    {
        { lk.lock() } -> std::same_as<void>;
        { lk.unlock() } -> std::same_as<void>;
    };

    struct Mutex
    {
        void lock() {
            std::println("Mutex::lock()");
        }

        void unlock() {
            std::println("Mutex::unlock()");
        }
    };


    template<typename Ty, Lockable LockType = std::mutex>
    struct Synchronized
    {
        using object_type = Ty;
        using reference   = object_type&;
        using pointer     = object_type*;

        explicit Synchronized(object_type& item): object { item } {
        }

        struct GuardProxy
        {
            constexpr GuardProxy(LockType& lk, reference obj):
                lock { lk }, object { obj } {
            }

            [[nodiscard]]
            pointer operator->() {
                return &object;
            }

            [[nodiscard]]
            reference operator*() {
                return object;
            }

        private:
            std::lock_guard<LockType> lock;
            reference object;
        };

        GuardProxy lock()
        {
            return GuardProxy { mtx, object };
        }


    private:
        reference object;
        LockType  mtx;
    };

    void demo()
    {
        Resource item { 1 };
        Synchronized<Resource, Mutex> synced {item};

        std::println("Value: {}", item.getValue());

        std::println("{}", std::string(120, '='));
        synced.lock()->increment();
        std::println("{}", std::string(120, '='));
        synced.lock()->increment();
        std::println("{}", std::string(120, '='));

        std::println("Value: {}", item.getValue());
    }
}


void synchronized_wrapper::TestAll()
{
    demo();
}

/**
Value: 1
========================================================================================================================
Mutex::lock()
Mutex::unlock()
========================================================================================================================
Mutex::lock()
Mutex::unlock()
========================================================================================================================
Value: 3
**/
