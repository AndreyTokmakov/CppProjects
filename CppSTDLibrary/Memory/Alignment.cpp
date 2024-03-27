/**============================================================================
Name        : Alignment.cpp
Created on  : 27.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Alignment.cpp
============================================================================**/

#include "Alignment.h"

#include "Alignment.h"

#include <memory>
#include <vector>
#include <new>
#include <cstring>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <vector>
#include <array>
#include <memory_resource>
#include <cstdlib> // for std::byte

#include "../Helpers/Helpers.h"

namespace Alignment
{

    class EmptyClass {
    };

    class Foo {
        int i;
        char a;
    };

#pragma pack(push, 1)
    struct FooAligned
    {
        int i;
        char a;
    };
#pragma pack(pop)

#pragma pack(push, 1)
    class Base_WithCharPrt {
    private:
        int value;
        char* prt;
    };
#pragma pack(pop)

#pragma pack(push, 1)
    class Base_WithFunc {
    private:
        int value;
    public:
        void Func() { std::cout << __FUNCTION__ << std::endl; }
    };
#pragma pack(pop)

#pragma pack(push, 1)
    class Base_WithVirtFunc {
    private:
        int value;
    public:
        virtual void Func() { std::cout << __FUNCTION__ << std::endl; }
    };
#pragma pack(pop)

    struct alignas(8) S {};
    struct alignas(1) U { S s; };

    void Alignment_Of_Tests()
    {
        std::cout << "alignment_of<EmptyClass>   = " << std::alignment_of<EmptyClass>::value << std::endl;
        std::cout << "size_of<EmptyClass>        = " << sizeof(EmptyClass) << std::endl;


        std::cout << "alignment_of<int>          = " << std::alignment_of<int>::value << std::endl;

        std::cout << "alignment_of_v<EmptyClass> = " << std::alignment_of_v<EmptyClass> << std::endl;
        std::cout << "alignment_of_v<double>     = " << std::alignment_of_v<int>          << std::endl;

        std::cout << "alignment_of_v<double>     = " << std::alignment_of_v<double>          << std::endl;
        std::cout << "alignment_of_v<Foo>        = " << std::alignment_of_v<Foo>             << std::endl;
        std::cout << "alignment_of_v<FooAligned> = " << std::alignment_of_v<FooAligned>      << std::endl;

        std::cout << "alignment_of<Base_WithCharPrt> = "  << std::alignment_of_v<Base_WithCharPrt>  << std::endl;
        std::cout << "alignment_of<Base_WithFunc> = "     << std::alignment_of_v<Base_WithFunc>     << std::endl;
        std::cout << "alignment_of<Base_WithVirtFunc> = " << std::alignment_of_v<Base_WithVirtFunc> << std::endl;

        std::cout << "char: " << std::alignment_of<char>::value << std::endl;
        std::cout << "int: " << std::alignment_of<int>::value << std::endl;
        std::cout << "int[20]: " << std::alignment_of<int[20]>::value << std::endl;
        std::cout << "long long int: " << std::alignment_of<long long int>::value << std::endl;

        std::cout << "alignment_of_v<S> = " << std::alignment_of_v<S>  << std::endl;
        std::cout << "alignment_of_v<U> = " << std::alignment_of_v<U>  << std::endl;
    }

    void AlignOf()
    {
        std::cout << alignof(std::max_align_t) << std::endl;
    }


//#pragma pack(push, 1)
    struct alignas(4) FooAligned2 {
        int i;
        char a;
    };
//#pragma pack(pop)

    void Alignas() {
        std::cout << "sizeof<Foo>  = " << sizeof(Foo) << std::endl;
        std::cout << "sizeof<FooAligned2>  = " << sizeof(FooAligned2) << std::endl;
    }

    //------------------------------------------------

    struct alignas(4) MyStruct1 {
        float x;
        float y;
        short z;
    };

    void Test() {
        // auto ptr = new Vec3[10];

        std::cout << sizeof(MyStruct1) << std::endl;
    }
}



namespace Alignment::Storage
{
    struct MyLong
    {
        long value {};

        explicit MyLong(long v): value {v}{
            std::cout << "MyLong(" << value << ")\n";
        }

        ~MyLong() {
            std::cout << "~MyLong(" << value << ")\n";
        }
    };


    template<typename T>
    class Uninitialized
    {
        std::aligned_storage_t<sizeof(T)> storage;

    public:
        template<typename... Args>
        void construct(Args&&... params) {
            new (&storage) T(std::forward<Args>(params)...);
            std::cout << "Data: " << reinterpret_cast<T*>(&storage)->value << std::endl;
        }

        ~Uninitialized() {
            reinterpret_cast<T*>(&storage)->~T();
        }
    };


    void Construct_Type()
    {
        Uninitialized<MyLong> longVal {};
        longVal.construct(5);
    }
}


namespace Alignment::NetworkHeaders
{
    struct ARPHeader final {
        uint16_t htype{0};
        uint16_t ptype{0};
        uint8_t hlen{};
        uint8_t plen{};
        uint16_t opcode{0};
        uint8_t sender_mac[6]{};
        uint32_t sender_ip{};
        uint8_t target_mac[6]{};
        uint32_t target_ip{};
    };

    struct alignas(1) ARPHeaderAligned final {
        uint16_t htype{0};
        uint16_t ptype{0};
        uint8_t hlen{};
        uint8_t plen{};
        uint16_t opcode{0};
        uint8_t sender_mac[6]{};
        uint32_t sender_ip{};
        uint8_t target_mac[6]{};
        uint32_t target_ip{};
    } __attribute__((packed, aligned(1))) ;


    void CheckAlignment()
    {
        std::cout << "ARPHeader:\n\tsize: " << sizeof(ARPHeader)
                  << ", Alignment: "<< alignof(ARPHeader)
                  << ", Alignment: "<< std::alignment_of_v<ARPHeader>
                  << std::endl;

        std::cout << std::endl;

        std::cout << "ARPHeaderAligned:\n\tsize: " << sizeof(ARPHeaderAligned)
                  << ", Alignment: "<< alignof(ARPHeaderAligned)
                  << ", Alignment: "<< std::alignment_of_v<ARPHeaderAligned>
                  << std::endl;

    }
}


namespace Alignment::AlignAs
{
    struct alignas(std::hardware_destructive_interference_size) Item
    {
        int32_t value {0};
        int64_t value2 {0};
        double ratio {0.0};
    };

    struct ItemNonAligned
    {
        int32_t value {0};
        int64_t value2 {0};
        double ratio {0.0};
    };

    void AlignToCacheLine()
    {
        std::cout << sizeof(ItemNonAligned) << std::endl;
        std::cout << sizeof(Item) << std::endl;
    }
}


void Alignment::TestAll()
{
    // Alignment_Of_Tests();

    // Alignas();
    AlignAs::AlignToCacheLine();

    // Storage::Construct_Type();

    // NetworkHeaders::CheckAlignment();
}