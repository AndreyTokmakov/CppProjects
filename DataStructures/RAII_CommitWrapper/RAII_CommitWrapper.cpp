/**============================================================================
Name        : RAII_CommitWrapper.cpp
Created on  : 26.11.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RAII_CommitWrapper
============================================================================**/

#include "RAII_CommitWrapper.h"

#include <iostream>
#include <functional>
#include <memory>

namespace
{
    struct Foo
    {
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

    struct Connection
    {
        std::string name {};

        explicit Connection(std::string&& s): name(std::move(s)) {
        }

        void commit() const noexcept {
            std::cout << "SUCCESS: Closing connection(" << name << ")\n";
        }

        void rollback() const noexcept {
            std::cout << "FAILURE: Drop connection(" << name << ")\n";
        }
    };
}


namespace RAII_CommitWrapper::One
{
    template<typename T>
    struct Wrapper final
    {
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


    void OpenConnection1(bool throwException = true)
    {
        Connection connection { "TestConnection" };

        /*
        Wrapper<Connection> wrapper {connection, [objPtr = &connection] { objPtr->close(); }};
        */

        CommitWrapper commitWrapper {[objPtr = &connection] { objPtr->commit(); },
                                     [objPtr = &connection] { objPtr->rollback(); }};

        if (throwException)
            throw std::exception();

        commitWrapper.success();

        // auto close = [ObjectPtr = &connection] { ObjectPtr->close(); };
        // close();
    }

    void Test()
    {
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
    struct ExecuteAdapter
    {
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

    void Test()
    {
        ExecuteAdapter<void, void, Foo> adapter {std::make_unique<Foo>(), &Foo::func1, &Foo::func2};
    }
}

namespace RAII_CommitWrapper::Three
{
    template<typename CommitFunc, typename RollbackFunc, class ObjectType>
    struct CommitAdapter
    {
        CommitAdapter(ObjectType *ptrObj, CommitFunc commitFunc, RollbackFunc rollbackFunc) :
                objectPtr{ptrObj}, commitCallback{commitFunc}, rollbackCallback{rollbackFunc}, succeeded{false} {
        }

        ~CommitAdapter() {
            if (succeeded)
                std::invoke(commitCallback, objectPtr);
            else
                std::invoke(rollbackCallback, objectPtr);
        }

        void success() noexcept {
            succeeded = true;
        }

    private:

        ObjectType *objectPtr{nullptr};
        CommitFunc commitCallback{};
        RollbackFunc rollbackCallback{};
        bool succeeded{false};
    };

    template<typename CommitFunc = decltype(&Connection::commit),
             typename RollbackFunc = decltype(&Connection::rollback),
             typename ObjectType = Connection>
    CommitAdapter<CommitFunc, RollbackFunc, ObjectType> getTransaction(Connection *connPtr)
    {
        return CommitAdapter{connPtr,
                             &Connection::commit,
                             &Connection::rollback
        };
    }

    void Test()
    {
        std::unique_ptr<Connection> session { std::make_unique<Connection>("SQL Session") };
        auto transaction = getTransaction(session.get());

        //return;
        transaction.success();
    }
}


void RAII_CommitWrapper::Test()
{
    // One::Test();
    // Two::Test();
    Three::Test();
}

