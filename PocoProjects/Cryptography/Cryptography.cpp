/**============================================================================
Name        : Cryptography.cpp
Created on  : 25.04.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Cryptography
============================================================================**/

#include "Cryptography.h"

#include <iostream>
#include "Poco/DigestStream.h"
#include "Poco/MD5Engine.h"


namespace Cryptography
{
    using Poco::DigestOutputStream;
    using Poco::DigestEngine;
    using Poco::MD5Engine;

    void test()
    {
        MD5Engine md5;
        DigestOutputStream ostr(md5);
        ostr << "This is some text";
        ostr.flush(); // Ensure everything gets passed to the digest engine
        const DigestEngine::Digest &digest = md5.digest(); // obtain result
        std::string result = DigestEngine::digestToHex(digest);

        std::cout << result << std::endl;
    }
}


void Cryptography::TestAll()
{
    test();
}