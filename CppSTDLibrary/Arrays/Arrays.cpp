//============================================================================
// Name        : Arrays.cpp
// Created on  : 20.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Arrays src
//============================================================================

#include "Arrays.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <cstring>


namespace Arrays {

    template<typename T>
    void print(const T* data, size_t size) {
        for (size_t i = 0; i < size; i++)
            std::cout << data[i] << " ";
        std::cout << std::endl;
    }

    template<typename T>
    void print(const T* data, size_t size, std::string_view info) {
        std::cout << info << " ";
        print(data, size);
    }
}

namespace Arrays::BasicArrays {

    void Create() {
        int data[] = {1,2,3,4,5,6,7,8,9};
        print(data, std::size(data), "Data: ");


        int data1[10] = { 0 };
        print(data1, std::size(data1), "Data1: ");
    }

    void MultidimentionalArrays() {
        unsigned int x = 10;
        unsigned int y = 5;
        int** data = new int* [x];

        for (unsigned int i = 0; i < x; ++i) {
            data[i] = new int[y];
        }

        for (unsigned int i = 0; i < x; i++) {
            for (unsigned int n = 0; n < y; n++) {
                data[i][n] = i * n;
            }
        }

        //std::cout << data[1][1] << std::endl;
        //std::cout << data[1][1] << std::endl;

        for (unsigned int i = 0; i < x; i++) {
            for (unsigned int n = 0; n < y; n++) {
                std::cout << data[i][n] << "  ";
            }
            std::cout << std::endl;
        }

        for (unsigned int i = 0; i < x; ++i) {
            delete[] data[i];
        }
        delete[] data;
    }

    void Initialyze () {
        {
            int data[10] = { 0 };
            print(data, std::size(data));
        }
        {
            int data[10] = { 0 };

            data[1] = 1;
            data[3] = 1;

            print(data, std::size(data));

            memset(data, 0, sizeof(data));

            print(data, std::size(data));
        }
    }

    void AccessElement() {
        int data[] = { 1,2,3,4,5,6,7,8,9 };
        std::cout << "data[3] = " << data[3] << std::endl;
        std::cout << "3[data] = " << 3[data] << std::endl;

        3[data] = 444;
        std::cout << "\ndata[3] = " << data[3] << std::endl;
        std::cout << "3[data] = " << 3[data] << std::endl;
    }

    void Access_And_Update_Element() {
        int data[] = { 1,2,3,4,5,6,7,8,9 };
        print(data, std::size(data));

        data[3] = 11;
        print(data, std::size(data));

        3[data] = 22;
        print(data, std::size(data));

        int &x = data[3];
        x = 33;
        print(data, std::size(data));

        int* x2 = &data[3];
        *x2 = 44;
        print(data, std::size(data));
    }

    void Sort() {
        int data[] = { 3,2,1 };

        print(data, std::size(data), "Before: ");
        std::sort(std::begin(data), std::end(data));
        print(data, std::size(data), "After: ");
    }

    void OutOrRangeError()
    {
        const int data[] = { 1,2,3,4,5,6,7,8,9 };
        print(data, std::size(data));
        std::cout << data[-53] << std::endl;
    }

    void ReInit_Test()
    {
        int data[10] = {0,1,2,3,4,5,6,7,8,9};

        for (auto i : data)
            std::cout << i << " ";
        std::cout << std::endl;

        memset(data, 0, sizeof(data));

        for (auto i : data)
            std::cout << i << " ";
        std::cout << std::endl;
    }

    void TEST() {
        std::array<int, 10> src {1,2,3,4,5,6,7,8,9,10};
        std::array<int, 10> dst {0,0,0,0,0,0,0,0,0,0};

        std::swap(src, dst);


        for (const auto& v : dst)
            std::cout << v << std::endl;

        for (const auto& v : src)
            std::cout << v << std::endl;

    }
}

void Arrays::TestAll()
{
    // BasicArrays::MultidimentionalArrays();

    // BasicArrays::Create();
    // BasicArrays::Initialyze();

    // BasicArrays::Sort();

    // BasicArrays::AccessElement();
    // BasicArrays::Access_And_Update_Element();

    // BasicArrays::OutOrRangeError();

    // BasicArrays::ReInit_Test();

    BasicArrays::TEST();
}
