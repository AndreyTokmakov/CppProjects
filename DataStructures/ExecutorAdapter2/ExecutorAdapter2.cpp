/**============================================================================
Name        : ExecutorAdapter2.cpp
Created on  : 30.08.2022
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : ExecutorAdapter2
============================================================================**/

#include "ExecutorAdapter2.h"

#include <iostream>
#include <memory>

namespace ExecutorAdapter2
{
    class ExecutorView
    {
    public:
        template<class ClassType>
        explicit ExecutorView(const ClassType& type): object { &type }, func_impl {[](const void* obj) {
            return static_cast<const ClassType*>(obj)->execute();
        }} { }

        void execute() {
            func_impl(object);
        }

    private:
        const void* object { nullptr };
        void (*func_impl)(const void*);
    };
};

namespace ExecutorAdapter2::Tests
{
    struct ClassA {
        void execute() const { std::cout << "ClassA::execute()"  << std::endl; }
    };

    struct ClassB {
        void execute() const { std::cout << "ClassB::execute()"  << std::endl; }
    };

    void invoke(ExecutorView view) {
        view.execute();
    }

    void tests() {
        invoke(ExecutorView{ClassA{}});
        invoke(ExecutorView{ClassB{}});
    }
}

void ExecutorAdapter2::TestAll()
{
    Tests::tests();

};


