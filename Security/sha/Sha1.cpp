/**============================================================================
Name        : Sha1.cpp
Created on  : 01.12.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Sha1.cpp
============================================================================**/

#include "Sha1.h"

#include "rsa.h"
#include "hex.h"
#include <iostream>


namespace CryptoCPP
{
    void generateSha()
    {
        // https://biicode-docs.readthedocs.io/c++/examples/crypto++.html
        CryptoPP::SHA1 sha1;
        std::string source = "Hello";  //This will be randomly generated somehow
        std::string hash = "";
        CryptoPP::StringSource ss(source, true,
                                  new CryptoPP::HashFilter(sha1,
                                                           new CryptoPP::HexEncoder(
                                                                   new CryptoPP::StringSink(hash))));
        std::cout << hash;
    }
}

void Sha1::TestAll()
{
    CryptoCPP::generateSha();
}