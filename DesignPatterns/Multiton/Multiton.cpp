/**============================================================================
Name        : Multiton.cpp
Created on  : 09.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Multiton.cpp
============================================================================**/

#include "Multiton.h"

#include <print>
#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <mutex>


/**
 * Multiton Pattern
 * In the world of software design patterns, the Multiton pattern stands out as a powerful tool for managing a
 * limited number of instances of a class.
 * While the Singleton pattern ensures a single instance, the Multiton pattern allows for multiple instances,
 * each identified by a unique key.
 * This pattern is particularly useful in scenarios where multiple instances of a class are needed, but their number
 * should be controlled and managed efficiently.
 */


/**
 * Applicability
 * Use the Multiton pattern when:
 * 1. You need to manage a fixed number of instances of a class.
 * 2. Each instance should be uniquely identified by a key.
 * 3. You want to ensure that the same instance is returned for a given key.
 */


namespace
{
    template<typename Derived>
    struct MultitonBase
    {
        using key_type = std::string;
        using value_type = std::shared_ptr<Derived>;


        template<typename ...Args>
        static value_type getInstance(const key_type& key, Args&& ... args)
        {
            const auto& [entry, ok] = instances.try_emplace(key);
            if (ok) {
                entry->second = std::make_shared<Derived>(std::forward<Args>(args)...);
            }
            return entry->second;
        }

    private:

        static inline std::map<key_type, value_type> instances {};
        static inline std::mutex mtx;
    };
}

namespace Demo
{
    struct Configuration: MultitonBase<Configuration>
    {
        explicit Configuration(const std::string& name) {
            std::println("Configuration({}) created", name);
        }
    };

    struct ConfigurationEx: MultitonBase<ConfigurationEx>
    {
        explicit ConfigurationEx(const std::string& name, int value) {
            std::println("Configuration({}, {}) created", name, value);
        }
    };

    void test()
    {
        using namespace std::string_literals;

        for (const std::string& cfg_name: { "Conf_1"s, "Conf_2"s, "Conf_1"s}){
            std::shared_ptr<Configuration> cfg = Configuration::getInstance(cfg_name, cfg_name);
        }

        for (const auto& [name, val]: std::vector<std::pair<std::string, int>>{
            {"Conf_1", 1}, {"Conf_2", 2}, {"Conf_1", 3}
        }){
            std::shared_ptr<ConfigurationEx> cfg = ConfigurationEx::getInstance(name, name, val);
        }
    }
}

void Multiton::TestAll()
{
    Demo::test();
}

/**
* Configuration(Conf_1) created
* Configuration(Conf_2) created
* Configuration(Conf_1, 1) created
* Configuration(Conf_2, 2) created
*/