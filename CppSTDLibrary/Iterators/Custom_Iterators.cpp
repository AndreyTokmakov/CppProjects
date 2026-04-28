/**============================================================================
Name        : Custom_Iterators.cpp
Created on  : 31.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Custom_Iterators.cpp
============================================================================**/

#include "Custom_Iterators.h"

#include <iostream>
#include <exception>
#include <vector>
#include <cassert>
#include <numeric>

namespace Iterators::CustomIterator
{

    template <typename T, size_t const Size>
    class dummy_array {
    private:
        T data[Size] = {};

    public:
        T const & GetAt(size_t const index) const {
            if (index < Size)
                return data[index];
            throw std::out_of_range("index out of range");
        }

        void SetAt(size_t const index, T const & value) {
            if (index < Size)
                data[index] = value;
            else
                throw std::out_of_range("index out of range");
        }

        size_t GetSize() const {
            return Size;
        }
    };

    template <typename T, typename C, size_t const Size>
    class dummy_array_iterator_type
    {
        size_t index;
        C& collection;

    public:
        dummy_array_iterator_type(C& collection, const size_t index) : index(index), collection(collection) {
        }

        bool operator!= (const dummy_array_iterator_type & other) const {
            return index != other.index;
        }

        const T& operator*() const {
            return collection.GetAt(index);
        }

        const dummy_array_iterator_type& operator++ () {
            ++index;
            return *this;
        }
    };


    template <typename T, size_t const Size>
    using dummy_array_iterator = dummy_array_iterator_type<T, dummy_array<T, Size>, Size>;

    template <typename T, size_t const Size>
    using dummy_array_const_iterator = dummy_array_iterator_type<T, dummy_array<T, Size> const, Size>;


    template <typename T, size_t const Size>
    inline dummy_array_iterator<T, Size> begin(dummy_array<T, Size>& collection) {
        return dummy_array_iterator<T, Size>(collection, 0);
    }

    template <typename T, size_t const Size>
    inline dummy_array_iterator<T, Size> end(dummy_array<T, Size>& collection) {
        return dummy_array_iterator<T, Size>(collection, collection.GetSize());
    }

    template <typename T, size_t const Size>
    inline dummy_array_const_iterator<T, Size> begin(dummy_array<T, Size> const & collection) {
        return dummy_array_const_iterator<T, Size>(collection, 0);
    }

    template <typename T, size_t const Size>
    inline dummy_array_const_iterator<T, Size> end(dummy_array<T, Size> const & collection) {
        return dummy_array_const_iterator<T, Size>(collection, collection.GetSize());
    }

    template <typename T, const size_t Size>
    void print_dummy_array(dummy_array<T, Size> const & arr) {
        for (auto && e : arr) {
            std::cout << e << std::endl;
        }
    }

    template <typename T, const size_t Size>
    void print_dummy_array_NonConst(dummy_array<T, Size> & arr) {
        for (auto && e : arr) {
            std::cout << e << std::endl;
        }
    }

    void Test()
    {
        dummy_array<int, 10> data;
        for (int i : {0, 1, 2, 3, 4, 5, 6, 7, 9})
            data.SetAt(i, i);

        // print_dummy_array(data);
        print_dummy_array_NonConst(data);
    }
}

namespace Iterators::CustomIterator2
{

    template <typename T>
    struct fixed_array
    {
        using size_type = size_t;

        class iterator {
        public:
            using value_type = T;
            using reference = T & ;
            using pointer = T * ;
            using self_type = iterator;
            // using iterator_category = std::forward_iterator_tag;
            // using difference_type = int;

            iterator(pointer ptr) : ptr_(ptr) {
            }

            self_type operator++() {
                self_type i = *this;
                ptr_++;
                return i;
            }

            self_type operator++(int junk) {
                ptr_++;
                return *this;
            }

            reference operator*() {
                return *ptr_;
            }

            pointer operator->() {
                return ptr_;
            }

            bool operator==(const self_type& rhs) {
                return ptr_ == rhs.ptr_;
            }

            bool operator!=(const self_type& rhs) {
                return ptr_ != rhs.ptr_;
            }

        private:
            pointer ptr_;
        };

        struct const_iterator
        {
            using value_type = T;
            using reference = T & ;
            using pointer = T * ;
            using self_type = const_iterator;
            using iterator_category = std::forward_iterator_tag;
            using difference_type = int;

            const_iterator(pointer ptr) : ptr_(ptr) {
            }

            self_type operator++() {
                self_type i = *this;
                ptr_++;
                return i;
            }

            self_type operator++(int junk) {
                ptr_++;
                return *this;
            }

            const reference operator*() {
                return *ptr_;
            }

            const pointer operator->() {
                return ptr_;
            }

            bool operator==(const self_type& rhs) {
                return ptr_ == rhs.ptr_;
            }

            bool operator!=(const self_type& rhs) {
                return ptr_ != rhs.ptr_;
            }
        private:
            pointer ptr_;
        };

        fixed_array(size_type size) : size_(size) {
            data_ = new T[size_];
        }

        size_type size() const noexcept {
            return size_;
        }

        T& operator[](size_type index) {
            assert(index < size_);
            return data_[index];
        }

        const T& operator[](size_type index) const {
            assert(index < size_);
            return data_[index];
        }

        iterator begin() {
            return iterator(data_);
        }

        iterator end() {
            return iterator(data_ + size_);
        }

        const_iterator begin() const {
            return const_iterator(data_);
        }

        const_iterator end() const {
            return const_iterator(data_ + size_);
        }

    private:
        T* data_;
        size_type size_;
    };

    //------------------------------------------------------------------------//

    void Test() {
        fixed_array<double> point3d(3);
        point3d[0] = 2.3;
        point3d[1] = 3.2;
        point3d[2] = 4.2;

        for (fixed_array<double>::iterator i = point3d.begin(); i != point3d.end(); i++)
            std::cout << *i << " ";
        std::cout << std::endl;

        std::vector<double> vec;
        // std::copy(point3d.begin(), point3d.end(), std::back_inserter(vec));

        for (std::vector<double>::iterator i = vec.begin(); i != vec.end(); i++)
            std::cout << *i << " ";

        std::cout << std::endl;
    }
}

namespace Iterators::CustomIterator3
{
    struct num_iterator
    {
        int i;

        explicit num_iterator(int position = 0) : i{position} {
        }

        num_iterator& operator++() noexcept {
            ++i;
            return *this;
        }

        int operator*() const noexcept { return i;
        }

        bool operator!=(const num_iterator &other) const noexcept {
            return i != other.i;
        }
    };


    class num_range
    {
        int a {};
        int b {};

    public:
        num_range(int from, int to) : a {from}, b {to} {
        }

        [[nodiscard]]
        inline num_iterator begin() const noexcept {
            return num_iterator {a};
        }

        [[nodiscard]]
        inline num_iterator end() const noexcept {
            return num_iterator {b};
        }
    };


    void Test()
    {
        for (int i : num_range{100, 110}) {
            std::cout << i << ", ";
        }
    }
};

namespace Iterators::CustomIterator_IntIterator
{
    class IntIterator
    {
        int value {0};

    public:
        explicit IntIterator(int v) : value{v} {
        }

        auto operator==(const IntIterator &it) const {
            return value == it.value;
        }

        auto operator!=(const IntIterator &it) const {
            return not (*this == it);
        }

        auto &operator*() const {
            return value;
        }

        auto &operator++() {
            ++value;
            return *this;
        }
    };

    void Test()
    {
        auto first = IntIterator{12}; // Start at 12
        auto last = IntIterator{16}; // Stop when equal to 16
        for (auto it = first; it != last; ++it) {
            std::cout << (*it) << " ";
        }
    }
}

namespace Iterators::CustomIterator_RangeIterator
{
    class Generator
    {
        int begin_ {0};
        int end_ {0};

    public:
        Generator(int begin, int end) : begin_{begin}, end_{end} {
        }

        class Iterator
        {
            using value_type  = int;
            using pointer     = int*;

            value_type value {0};

        public:
            explicit Iterator(value_type pos) : value {pos} {
            }

            value_type operator*() const noexcept {
                return value;
            }

            pointer operator->() noexcept {
                return &value;
            }

            Iterator& operator++() {
                ++value;
                return *this;
            }

            Iterator operator++(int) {
                Iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const Iterator &other) const noexcept {
                return value == other.value;
            }

            bool operator!=(const Iterator& other) const noexcept {
                return value != other.value;
            }

            /*
            friend bool operator==(const Iterator& fir, const Iterator& sec) {
                return fir.value_ == sec.value_;
            }
            friend bool operator!=(const Iterator& fir, const Iterator& sec) {
                return fir.value_ != sec.value_;
            }
            */
        };

        [[nodiscard]]
        Iterator begin() const noexcept {
            return Iterator {begin_};
        }

        [[nodiscard]]
        Iterator end() const noexcept {
            return Iterator {end_};
        }
    };

    void Test()
    {
        const Generator gen{1, 10};
        for (auto v : gen)
            std::cout << v << " ";

        std::cout << "\nsum:  " << std::accumulate(std::begin(gen), std::end(gen), 0);

        std::cout << "\nprod: " << std::accumulate(gen.begin(), gen.end(), 1,
                                                   [](int fir, int sec){ return fir * sec; });
    }
}

namespace Iterators::making_types_iterable
{
    struct RingBuffer
    {
        std::array<float, 256> data {};
        uint32_t start = 0;
        uint32_t count = 0;

        // Raw pointers are perfectly valid iterators
        float* begin() {
            return data.data() + start;
        }

        float* end() {
            return data.data() + start + count;
        }
    };

    RingBuffer getBuffer(const int size)
    {
        RingBuffer buffer {};
        std::iota(buffer.data.begin(), buffer.data.begin() + size, 1);
        buffer.start = 0;
        buffer.count = size;
        return buffer;
    }

    struct DataPacket
    {
        std::array<uint8_t, 256> payload {};
        size_t length { 0U };
    };

    // Place these in the same namespace — ADL will find them
    const uint8_t* begin(const DataPacket& pkt)
    {
        return pkt.payload.data();
    }

    const uint8_t* end(const DataPacket& pkt)
    {
        return pkt.payload.data() + pkt.length;
    }

    DataPacket receive(const int len)
    {
        DataPacket packet {};
        std::iota(packet.payload.data(), packet.payload.data() + len, 1);
        packet.length = len;
        return packet;
    }

    void demo_1()
    {
        RingBuffer buf = getBuffer(12);
        for (float const sample : buf) {
            std::cout << sample << " ";
        }
        // 1 2 3 4 5 6 7 8 9 10 11 12
    }

    void demo_2()
    {
        const DataPacket pkt = receive(32);
        for (const uint8_t byte : pkt) {
            std::cout << static_cast<int>(byte) << " ";
        }
        // 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32
    }
}

void Iterators::Custom_Iterators::TestAll()
{
    // CustomIterator::Test();
    // CustomIterator2::Test();
    // CustomIterator3::Test();
    // CustomIterator_IntIterator::Test();
    // CustomIterator_RangeIterator::Test();

    // making_types_iterable::demo_1();
    making_types_iterable::demo_2();
}