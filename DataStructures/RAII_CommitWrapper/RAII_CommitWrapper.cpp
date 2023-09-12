//============================================================================
// Name        : RAII_CommitWrapper.cpp
// Created on  : 16.06.22.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : RAII_CommitWrapper
//============================================================================

#include "RAII_CommitWrapper.h"

#include <iostream>
#include <functional>
#include <memory>

namespace RAII_CommitWrapper::One
{
    struct Connection {
        std::string name {};

        explicit Connection(std::string&& s): name(std::move(s)) {
        }

        void close() const noexcept {
            std::cout << "SUCCESS: Closing connection(" << name << ")\n";
        }

        void drop() const noexcept {
            std::cout << "FAILURE: Drop connection(" << name << ")\n";
        }
    };

    template<typename T>
    struct Wrapper final {
        T& objRef {};
        std::function<void(void)> callback {};

        explicit Wrapper(T& v, std::function<void(void)> func):
                objRef {v}, callback {std::move(func)} {
        }

        ~Wrapper() {
            std::cout << "DTor called\n";
            callback();
        }
    };

    template<typename F1, typename F2>
    struct CommitWrapper final
    {
        F1 successCallback {};
        F2 failureCallback {};
        bool ok { false };

        explicit CommitWrapper(F1&& func1, F2&& func2):
                successCallback {std::move(func1)}, failureCallback {std::move(func2)} {
        }

        void success() noexcept {
            ok = true;
        }

        ~CommitWrapper()
        {
            std::cout << "DTor called\n";
            if (ok)
                successCallback();
            else
                failureCallback();
        }
    };


    void OpenConnection1(bool throwException = true) {
        Connection connection { "TestConnection" };

        /*
        Wrapper<Connection> wrapper {connection, [objPtr = &connection] { objPtr->close(); }};
        */

        CommitWrapper commitWrapper {[objPtr = &connection] { objPtr->close(); },
                                     [objPtr = &connection] { objPtr->drop(); }};

        if (throwException)
            throw std::exception();

        commitWrapper.success();

        // auto close = [ObjectPtr = &connection] { ObjectPtr->close(); };
        // close();
    }

    void Test() {
        try {
            OpenConnection1();
        } catch (...) {
            std::cout << " * * * * * Exception * * * * * \n";
        }

        std::cout << "---------------------------------------------------------------------\n";

        try {
            OpenConnection1(false);
        } catch (...) {
            std::cout << " * * * * * Exception * * * * * \n";
        }
    }
}


namespace RAII_CommitWrapper::Two
{
    template <typename Ret1, typename Ret2, class Type>
    class ExecuteAdapter {
    public:
        ExecuteAdapter(std::unique_ptr<Type> obj, Ret1(Type::*method1)(), Ret2(Type::*method2)()):
                object { std::move(obj) }, inCaseOfSuccess { method1 }, inCaseOfFailure { method2 } {
        }

        ~ExecuteAdapter()
        {
            std::invoke(inCaseOfSuccess, object);
            std::invoke(inCaseOfFailure, object);
        }

    private:
        std::unique_ptr<Type> object { nullptr };
        Ret1 (Type::*inCaseOfSuccess)();
        Ret2 (Type::*inCaseOfFailure)();

        bool ok {false};
    };

    class Foo
            {
    public:
        ~Foo() {
            std::cout << "~Foo::Foo()" << std::endl;
        }

        void func1() {
            std::cout << "Foo::func1()" << std::endl;
        }

        void func2() {
            std::cout << "Foo::func2()" << std::endl;
        }
    };

    void Test()
    {
        ExecuteAdapter<void, void, Foo> adapter {std::make_unique<Foo>(), &Foo::func1, &Foo::func2};
    }
}

void RAII_CommitWrapper::TEST_ALL()
{
    // One::Test();
    Two::Test();
}

