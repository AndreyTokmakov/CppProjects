/**============================================================================
Name        : PerfectForwarding.cpp
Created on  : 14.05.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PerfectForwarding
============================================================================**/

#include "Templates.hpp"

#include <iostream>
#include <print>
#include <string>
#include <utility>

#include "../Helpers/Integer.h"

namespace
{
    template<class T>
    void __print_no_forward(T&& var) {
        var.printInfo();
    }

    template<class T>
    void print_no_forward(T&& var) {
        __print_no_forward(var);
    }

    template<class T>
    void __print(T&& var) {
        var.printInfo();
    }

    template<class T>
    void print(T&& var) {
        __print(std::forward<T>(var));
    }

    template<class... Args>
    void __print_integers(Args&&... args) {
        (args.printInfo(), ...);
        (args.increment(), ...);
    }

    template<class... Args>
    void print_integers(Args&&... args) {
        __print_integers(std::forward<Args>(args) ... );
    }

    void Test_No_Forward()
    {
        Integer integer(111);
        print_no_forward(integer);
    }

    void Test_Integer()
    {
        Integer integer(123);
        print(integer);
    }

    void Test_IntegerList()
    {
        Integer integer1(11);
        Integer integer2(22);

        print_integers(integer1, integer2);
    }
}

namespace
{
    class Str
    {
        std::string value;

    public:

        template<typename ... Args>
        Str(Args&& ... params) : value(std::forward<Args>(params)...) {
            std::cout << "[Object_ToMove constructor 2] (" << this->value << ")" << std::endl;
        }


        Str(const Str &obj) {
            std::cout << "[Copy constructor] (" << this->value << ")" << std::endl;
            this->value = obj.value;
        }

        Str& operator=(const Str& right) {
            std::cout << "[Copy assignment operator] (" << this->value << " -> " << right.value << ")" << std::endl;
            if (this != &right)
                this->value = right.value;
            return *this;
        }

        Str(Str&& obj) noexcept : value(std::move(obj.value)) {
            std::cout << "[Move constructor] (" << this->value << ")" << std::endl;
        }

        Str& operator=(Str&& right) noexcept {
            std::cout << "[Move assignment operator] (" << this->value << " => " << right.value << ")" << std::endl;
            if (this != &right)
                this->value = std::move(right.value);
            return *this;
        }

        virtual ~Str() {
            std::cout << "[Destructor] (" << this->value << ")" << std::endl;
        }

        [[nodiscard]]
        virtual std::string getValue() const noexcept {
            return this->value;
        }

        virtual void printInfo() const noexcept {
            std::cout << "Object_ToMove = " << this->value << std::endl;
        }
    };

    void Construct_Object() {
        {
            Str str("Some_Test_String");
            str.printInfo();
            Str str1 = std::move(str);
        }
        {
            const Str str("Some_Test_String");
            str.printInfo();
#if 0
            // ERROR HERE. Cant construct string using std::move
			Str str1 = std::move(str);
#endif
        }
    }
}


namespace
{
    struct Logger
    {
        // Variadic template method - accepts any number of arguments
        template<typename... Args>
        void log(Args&&... args)
        {
            std::print("[LOG] ");
            log_impl(std::forward<Args>(args)...);
            std::println();
        }

    private:
        // Base case: no arguments (stops recursion)
        static void log_impl() {}

        // Recursive case: print first argument, recurse with rest
        template<typename First, typename... Rest>
        void log_impl(First&& first, Rest&&... rest)
        {
            std::print("{} ", std::forward<First>(first));;
            log_impl(std::forward<Rest>(rest)...);  // Recursive call with remaining args
        }
    };

    void loggerDemo()
    {
        Logger logger;

        // Multiple arguments of different types
        logger.log("Error:", 404, "Not Found");

        // Mixed types (string, double, string)
        logger.log("Temperature:", 23.5, "degrees");

        // Single argument
        logger.log("Single message");

        // Many arguments
        logger.log("User", "Alice", "logged in at", 14, ":", 30);

        // With lvalues and rvalues
        std::string msg = "Warning";
        int code = 500;
        logger.log(msg, code, "Server error");  // lvalues
        logger.log("Info", 200, "OK");          // rvalues
    }

    /**
    [LOG] Error: 404 Not Found
    [LOG] Temperature: 23.5 degrees
    [LOG] Single message
    [LOG] User Alice logged in at 14 : 30
    [LOG] Warning 500 Server error
    [LOG] Info 200 OK
    **/
}

void Templates::perfect_forwarding::TestAll()
{
    loggerDemo();

    Test_No_Forward();
    Test_Integer();
    Test_IntegerList();
    Construct_Object();
}
