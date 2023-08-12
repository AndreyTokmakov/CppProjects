//============================================================================
// Name        : Array.h
// Created on  : 05.07.22.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Array
//============================================================================

#include "Array.h"

#include <iostream>
#include <memory>
#include <algorithm>
#include <numeric>
#include <concepts>
#include <utility>


namespace Array {

    template <typename T = int >
        requires std::default_initializable<T> && std::destructible<T>
    class Array {
    private:
        using value_type = T;
        using size_type = size_t;

        static_assert(!std::is_same_v<value_type, void>,
                      "Type of the value shall not be void");

        value_type *data { nullptr };
        size_type size { 0 };

    public:
        explicit Array(size_type s) : data{new value_type[s]}, size{s} {
        }

        value_type &operator[](size_type idx) {
            return data[idx];
        }

        const value_type &operator[](size_type idx) const {
            return data[idx];
        }

        inline value_type *Data() noexcept {
            return data;
        }

        [[nodiscard]]
        inline size_type Size() const noexcept {
            return size;
        }

        Array(const Array &other) : data { new value_type[other.size] }, size { other.size } {
            std::copy_n(other.data, size, data);
        }

        Array(Array &&other) noexcept:
                data { std::exchange(other.data, nullptr) },
                size { std::exchange(other.size, 0) } {
        }

        Array &operator=(const Array &other) {
            if (&other == this)
                return *this;

            Array localCopy(other);
            // swap(localCopy);
            Array::swap(localCopy, *this);
            return *this;
        }

        Array &operator=(Array &&other) noexcept {
            if (this != &other) {
                /// delete[] data; ????
                data = std::exchange(other.data, nullptr);
                size = std::exchange(other.size, 0);
            }
            return *this;
        }

        /*
        [[nodiscard]]
        inline T begin() const noexcept {
            return data[0];
        }

        [[nodiscard]]
        inline T end() const noexcept {
            return data[size - 1];
        }
        */

        void swap(Array &other) noexcept {
            std::swap(this->data, other.data);
            std::swap(this->size, other.size);
        }

        static void swap(Array &first, Array &second) noexcept {
            std::swap(first.data, second.data);
            std::swap(first.size, second.size);
        }

        ~Array() {
            std::cout << "Array::~Array()\n";
            delete[] data;
            data = nullptr;
            size = 0;
        }
    };


    template <typename T, typename Coll>
    class IteratorBase {
    private:
        using value_type = T;
        using size_type  = size_t;
        using collection_type  = Coll;

        size_type index {0};
        collection_type& collection;

    public:
        IteratorBase(collection_type& collection, const size_type index) :
                index { index }, collection { collection } {
        }

        inline bool operator!=(const IteratorBase& other) const noexcept {
            return index != other.index;
        }

        inline const value_type& operator*() const noexcept {
            return collection[index];
        }

        const IteratorBase& operator++() noexcept {
            ++index;
            return *this;
        }
    };

    template <typename T>
    using ArrayIterator = IteratorBase<T, Array<T>>;

    template <typename T>
    using ArrayConstIterator = IteratorBase<T, const Array<T>>;


    template <typename T>
    inline ArrayIterator<T> begin(Array<T>& collection) {
        return ArrayIterator<T> (collection, 0);
    }

    template <typename T>
    inline ArrayIterator<T> end(Array<T>& collection) {
        return ArrayIterator<T> (collection, collection.Size());
    }

}

namespace Array::Tests {

    void CreateTest() {

        struct Obj {
            int v {0};
            explicit Obj(int i): v {i} {
            }
        };

        /** Concept constrains violation **/
        // Array<Obj> numbers {3};

        Array<int> numbers {3};
    }


    void Test() {
        Array<int> numbers {3};

        numbers[2] = 4;

        for (int i = 0; i < numbers.Size(); ++i) {
            std::cout << numbers[i] << std::endl;
        }

        std::cout << "--------------------------------------------\n";

        Array<int> numbers1 (numbers);
        for (int i = 0; i < numbers1.Size(); ++i) {
            std::cout << numbers1[i] << std::endl;
        }

        std::cout << "--------------------------------------------\n";

        Array<int> numbers2(3);
        numbers2 = numbers1;
        for (int i = 0; i < numbers2.Size(); ++i) {
            std::cout << numbers2[i] << std::endl;
        }

        std::cout << "--------------------------------------------\n";

        numbers2 = numbers2;
        for (int i = 0; i < numbers2.Size(); ++i) {
            std::cout << numbers2[i] << std::endl;
        }
    }

    void MoveConstructor_Test() {
        Array<int> numbers {5};

        std::iota(numbers.Data(), numbers.Data() + numbers.Size(), 1);
        for (size_t i = 0; i < numbers.Size(); ++i)
            std::cout << numbers[i] << std::endl;

        std::cout << "--------------------------------------------\n";

        Array<int> numbers2 = std::move(numbers);
        for (size_t i = 0; i < numbers2.Size(); ++i)
            std::cout << numbers2[i] << std::endl;

        std::cout << "--------------------------------------------\n";

        std::cout << numbers.Size() << " " << numbers2.Size() << std::endl;
    }

    void MoveAssignment_Test() {
        Array<int> numbers1 {5}, numbers2 {5};

        std::iota(numbers1.Data(), numbers1.Data() + numbers1.Size(), 1);
        std::iota(numbers2.Data(), numbers2.Data() + numbers2.Size(), 5);

        auto printArrays = [](const Array<int>& array1, const Array<int>& array2) {
            const auto maxSize = std::max(array1.Size(), array2.Size());
            for (size_t i = 0; i < maxSize; ++i) {
                if (array1.Size() > i) std::cout << array1[i];
                std::cout << "     ";
                if (array2.Size() > i) std::cout << array2[i];
                std::cout << std::endl;
            }
        };

        printArrays(numbers1, numbers2);

        std::cout << "---------------- move test ----------------------------\n";

        numbers1 = std::move(numbers2);
        printArrays(numbers1, numbers2);

        std::cout << "--------------- self assignment -----------------------------\n";

        numbers1 = std::move(numbers1);
        printArrays(numbers1, numbers2);

        std::cout << "--------------------------------------------\n";
    }


    void SelfAssignment() {
        Array<int> numbers1 {5};

        std::iota(numbers1.Data(), numbers1.Data() + numbers1.Size(), 1);

        numbers1 = numbers1;

        std::cout << "Done\n";
    }

    void IteratorsTest() {
        Array<int> numbers {10};
        std::iota(numbers.Data(), numbers.Data() + numbers.Size(), 1);

        for (auto && e : numbers) {
            std::cout << e << std::endl;
        }
    }
}

void Array::TEST_ALL()
{
    // Tests::CreateTest();

    // Tests::Test();
    // Tests::MoveConstructor_Test();
    Tests::SelfAssignment();
    // Tests::MoveAssignment_Test();
    // Tests::IteratorsTest();
}
