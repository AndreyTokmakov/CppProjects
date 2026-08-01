/**============================================================================
Name        : Test_MethodPtr.cpp
Created on  : 03.01.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Test_MethodPtr pattern src
============================================================================**/

#include <algorithm>

#include "Command.hpp"

#include <iostream>
#include <string>
#include <functional>


namespace
{
    template <class Type>
    class Command
    {
        Type *object { nullptr };
        void (Type::*method)() { nullptr };

    public:
        Command() = default;
        Command(Type *obj, void(Type::*m)()): object {obj}, method {m} {
        }

        void execute() {
            // TODO: std::invoke or call directly....
            // (object->*method)();
            std::invoke(method, object);
        }
    };

    class Person
    {
        std::string name {};
        Command<Person> cmd {};

    public:
        explicit Person(std::string n): name { std::move(n) } {
        }

        void talk()
        {
            std::cout << name << " is talking" << std::endl;
        }

        void listen() {
            std::cout << name << " is listening" << std::endl;
        }
    };

    void Test()
    {
        Person wilma("Wilma" );
        Person betty("Betty");

        using Cmd = Command<Person>;

        std::cout << sizeof(Cmd) << std::endl;

        std::vector<Cmd> commands {};

        commands.emplace_back(&wilma, &Person::listen);
        commands.emplace_back(&betty, &Person::talk);
        commands.emplace_back(&betty, &Person::listen);
        commands.emplace_back(&wilma, &Person::listen);

        std::ranges::for_each(commands, [](Cmd cmd) {  cmd.execute(); });

    }
}

void command::method_ptr::TestAll()
{
    Test();

    // Wilma is listening
    // Betty is talking
    // Betty is listening
    // Wilma is listening
}


