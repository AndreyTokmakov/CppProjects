/**============================================================================
Name        : PasskeyIdiom.cpp
Created on  : 14.11.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PasskeyIdiom.cpp
============================================================================**/

#include <string>
#include "PasskeyIdiom.hpp"

#include <iostream>
#include <ostream>

namespace simple_example
{

    class Token
    {
        Token() = default;
        friend struct AllowedClass;
        friend void functionWithAccess();
    };

    void modifyPrivateData(Token)
    {
        std::cout << "modifying private data" << std::endl;
    }

    struct AllowedClass
    {
        void execute() {
            doSomethingSecret(Token{});
        }

    private:

        void doSomethingSecret(Token)
        {
            std::cout << "doing something secret" << std::endl;
        }
    };

    void functionWithAccess()
    {
        modifyPrivateData(Token{});
    }

    void functionWithoutAccess()
    {
        // modifyPrivateData(Token{});  <<----- Will not compile
                                        // simple_example::Token::Token()’ is private within this context
    }

    void test()
    {
        AllowedClass{}.execute();
        functionWithAccess();
        functionWithoutAccess();
    }
}

namespace demo2
{
    class Secret
    {
        class AccessKey
        {
            friend struct SecretFactory;

            AccessKey() = default;
            AccessKey(AccessKey const&) = default;
        };

    public:

        // Whoever can provide a key has access:
        explicit Secret(std::string str, AccessKey) : data(std::move(str)) {
            std::cout << "Secret::Secret(" << data << ")" << std::endl;
        }

    private:

        // these stay private, since Secret itself has no friends anymore
        void addData(const std::string& moreData) {
            std::cout << "Secret::addData(" << moreData << ")" << std::endl;
        }

        std::string data {};
    };

    // 1. Do not have access to Secret private data
    // 2. yet can create Secret objects
    // 3. .. and Secret can be created ONLY buy  SecretFactory
    struct SecretFactory
    {
        Secret getSecret([[maybe_unused]] std::string str)
        {
            // OK, SecretFactory can access
            return Secret { std::move(str), {} };
        }

        void modify([[maybe_unused]] Secret& secret,
                    [[maybe_unused]] const std::string& additionalData)
        {
            /** ERROR: void Secret::addData(const string&) is private **/
            // secret.addData(additionalData);
        }
    };

    void test()
    {
        /** ERROR: Secret::AccessKey::AccessKey() is private **/
        // Secret s1 { "foo?", {} };

        SecretFactory sf;
        Secret s2 = sf.getSecret("moo!"); //OK
    }
}

// NOTE: https://arne-mertz.de/2016/10/passkey-idiom/

void PasskeyIdiom::TestAll()
{
    // simple_example::test();
    demo2::test();
}
