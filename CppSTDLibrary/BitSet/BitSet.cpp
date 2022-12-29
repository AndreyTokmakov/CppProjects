//============================================================================
// Name        : BitSet.cpp
// Created on  : 24.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : BitSet src
//============================================================================

#include <iostream>       // std::cout
#include <string>         // std::string
#include <bitset>         // std::bitset

#include "BitSet.h"

namespace BitSet
{
    void ToString() {
        std::bitset<4> mybits;     // mybits: 0000
        mybits.set();              // mybits: 1111

        std::string mystring = mybits.to_string<char, std::string::traits_type, std::string::allocator_type>();

        std::cout << "mystring: " << mystring << std::endl;
    }

    void ToULong()
    {
        std::bitset<4> foo;     // foo: 0000
        foo.set();              // foo: 1111
        std::cout << foo << " as an integer is: " << foo.to_ulong() << std::endl;
    }

    void Flip()
    {
        std::bitset<4> foo(std::string("0001"));

        std::cout << foo.flip(2) << std::endl;    // 0101
        std::cout << foo.flip() << std::endl;     // 1010
    }

    void Reset()
    {
        std::bitset<4> foo(std::string("1011"));

        std::cout << foo.reset(1) << std::endl;      // 1001
        std::cout << foo.reset()  << std::endl;       // 0000
    }

    void Set()
    {
        std::bitset<4> foo;

        std::cout << foo.set() << std::endl;      // 1111
        std::cout << foo.set(2, 0) << std::endl;  // 1011
        std::cout << foo.set(2) << std::endl;     // 1111
    }

    void Test()
    {
        std::bitset<5> foo(std::string("01011"));

        std::cout << "foo contains: "<< std::endl;
        std::cout << std::boolalpha;
        for (std::size_t i = 0; i < foo.size(); ++i)
            std::cout << foo.test(i) << std::endl;
    }

    void Set_and_Test()
    {
        std::bitset<4> foo;
        int bit { 2 };

        std::cout << foo.test(bit) << std::endl;
        foo.set(bit);
        std::cout << foo.test(bit) << std::endl;
    }

    void Count()
    {
        std::bitset<8> foo(std::string("10110011"));

        std::cout << foo << " has ";
        std::cout << foo.count() << " ones and ";
        std::cout << (foo.size() - foo.count()) << " zeros.\n";
    }

    void Operators()
    {
        std::bitset<4> foo(std::string("1001"));
        std::bitset<4> bar(std::string("0011"));

        std::cout << (foo | bar) << std::endl;
        std::cout << "foo: " << foo << ", bar: " << bar << std::endl;

        std::cout << "\n(foo ^= bar): " << (foo ^= bar) << std::endl;
        std::cout << "foo: " << foo << ", bar: " << bar << std::endl;

        std::cout << "\n(foo &= bar): " << (foo &= bar) << std::endl;
        std::cout << "foo: " << foo << ", bar: " << bar << std::endl;

        std::cout << "\n(foo |= bar): " << (foo |= bar) << std::endl;
        std::cout << "foo: " << foo << ", bar: " << bar << std::endl;

        std::cout << "\n(foo <<= 2) " << (foo <<= 2) << std::endl;      // 1100 (SHL,assign)
        std::cout << "foo: " << foo << ", bar: " << bar << std::endl;

        std::cout << "\n(foo >>= 1) " << (foo >>= 1) << std::endl;      // 0110 (SHR,assign)
        std::cout << "foo: " << foo << ", bar: " << bar << std::endl;

        /*
        std::cout << (~bar) << '\n';           // 1100 (NOT)
        std::cout << (bar << 1) << '\n';         // 0110 (SHL)
        std::cout << (bar >> 1) << '\n';         // 0001 (SHR)

        std::cout << (foo == bar) << '\n';       // false (0110==0011)
        std::cout << (foo != bar) << '\n';       // true  (0110!=0011)

        std::cout << (foo&bar) << '\n';        // 0010
        std::cout << (foo | bar) << '\n';        // 0111
        std::cout << (foo^bar) << '\n';        // 0101
        */
    }

    void CheckBits() {
        std::bitset<1024> mybits;     // mybits: 0000

        std::cout << "sizeof(mybits) = " << sizeof(mybits) << std::endl;
        std::cout << "mybits.size() = " << mybits.size() << std::endl;

        mybits.set(1000);
        std::cout << mybits.test(1000) << std::endl;
    }

    void All()
    {
        std::bitset<8> foo;

        std::cout << "Please, enter an 8-bit binary number: ";
        std::cin >> foo;
        std::cout << foo << ". (" << foo.to_ulong() << ")" << std::endl;


        std::cout << std::boolalpha;
        std::cout << "all: " << foo.all() << std::endl;
        std::cout << "any: " << foo.any() << std::endl;
        std::cout << "none: " << foo.none() << std::endl;
    }
};

namespace BitSet
{
    void TestAll() {
        // Set();
        // Test();
        Set_and_Test();
        // Count();

        // CheckBits();

        // Operators();
        // All();

        // ToString();
        // ToULong();
        // Flip();
        // Reset();
    }
};