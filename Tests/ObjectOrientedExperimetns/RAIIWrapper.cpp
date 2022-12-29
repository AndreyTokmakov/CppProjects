/**============================================================================
Name        : RAIIWrapper.cpp
Created on  : 11.06.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : RAIIWrapper
============================================================================**/

#include "RAIIWrapper.h"

#include <iostream>
#include <functional>

namespace ObjectOrientedExperiments::RAIIWrapper {

    struct Connection {
        std::string name {};

        explicit Connection(std::string&& s): name(std::move(s)) {
        }

        void close() {
            std::cout << "closing connection(" << name << ")\n";
        }

        void drop() {
            std::cout << "drop connection(" << name << ")\n";
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
    struct CommitWrapper final {
        F1 successCallback {};
        F2 failureCallback {};
        bool ok {false};

        explicit CommitWrapper(F1&& func1, F2&& func2):
                successCallback {std::move(func1)}, failureCallback {std::move(func2)} {
        }

        void success() noexcept {
            ok = true;
        }

        ~CommitWrapper() {
            std::cout << "DTor called\n";
            if (ok)
                successCallback();
            else
                failureCallback();
        }
    };

    //=================================================================

    void OpenConnection() {
        Connection connection { "TestConnection" };

        /*
        Wrapper<Connection> wrapper {connection, [objPtr = &connection] { objPtr->close(); }};
        */

        CommitWrapper commitWrapper {[objPtr = &connection] { objPtr->close(); },
                                     [objPtr = &connection] { objPtr->drop(); }};

        throw std::exception();

        commitWrapper.success();

        // auto close = [ObjectPtr = &connection] { ObjectPtr->close(); };
        // close();
    }


    void Test() {
        try {
            OpenConnection();
        } catch (...) {
            std::cout << " * * * * * Exception * * * * * \n";
        }

    }
}


void ObjectOrientedExperiments::RAIIWrapper::TestAll() {
   RAIIWrapper::Test();
};

