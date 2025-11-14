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

namespace simple_examples
{
    class Secret
    {
        class ConstructorKey
        {
            friend struct SecretFactory;

            ConstructorKey() = default;
            ConstructorKey(ConstructorKey const&) = default;
        };

    public:

        // Whoever can provide a key has access:
        explicit Secret(std::string str, ConstructorKey) : data(std::move(str)) {
        }

    private:

        // these stay private, since Secret itself has no friends anymore
        void addData(const std::string& moreData) {
        }

        std::string data {};
    };

    struct SecretFactory
    {
        Secret getSecret(std::string str)
        {
            // OK, SecretFactory can access
            return Secret { std::move(str), {} };
        }

        void modify(Secret& secret, std::string const& additionalData)
        {
            // ERROR: void Secret::addData(const string&) is private
            // secret.addData(additionalData);
        }
    };

    void test()
    {
        /** ERROR: Secret::ConstructorKey::ConstructorKey() is private **/
        // Secret s { "foo?", {} };

        SecretFactory sf;
        Secret s = sf.getSecret("moo!"); //OK
    }
}

// NOTE: https://arne-mertz.de/2016/10/passkey-idiom/

void PasskeyIdiom::TestAll()
{

}
