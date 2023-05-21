//============================================================================
// Name        : LinkedList.h
// Created on  : 13.03.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : LinkedList
//============================================================================

#include "LinkedList.h"
#include <iostream>
#include <memory>
#include <list>
#include <utility>


namespace LinkedLists::One {

    template<typename T>
    struct Node {
        T data {};
        Node *next {nullptr};

        Node (T v, Node<T>* nxt = nullptr): data {v}, next {nxt} {

        }
    };


    template<typename T>
    struct LinkedList
    {
        using object_type = T;
        using reference = object_type&;
        using pointer = object_type*;
        using const_reference = const object_type&;
        using const_pointer = const object_type*;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        Node<object_type> *head { nullptr };
        Node<object_type> *tail { nullptr };

        // TODO: Increment
        size_t size {0};

    public:
        class ListIterator {
            using value_type = LinkedList::object_type;
            using reference = LinkedList::reference;
            using pointer = LinkedList::pointer;
            // using self_type = ListIterator;

            Node<value_type> *ptr;

            template<typename Type>
            friend struct LinkedList;

            [[maybe_unused]]
            explicit ListIterator(Node<value_type> *p) : ptr{p} {
                //std::cout << "ListIterator!!!\n";
            }

        public:
            reference operator*() const {
                return ptr->data;
            }

            ListIterator &operator++() {
                ptr = ptr->next;
                return *this;
            }

            ListIterator operator++(int) {
                ListIterator iter = *this;
                ++*this;
                return iter;
            }

            bool operator==(const ListIterator &rhs) const {
                return ptr == rhs.ptr;
            }

            bool operator!=(const ListIterator &rhs) const {
                return ptr != rhs.ptr;
            }
        };

        class ListConstIterator {
            using value_type = LinkedList::object_type;
            using const_reference = LinkedList::const_reference;
            using const_pointer = LinkedList::const_pointer;

            Node<value_type> *ptr;

            template<typename Type>
            friend struct LinkedList;

            [[maybe_unused]]
            explicit ListConstIterator(Node<value_type> *p) : ptr{p} {
                //std::cout << "ListConstIterator!!!\n";
            }

        public:
            const_reference operator*() const {
                return ptr->data;
            }

            const_pointer operator->() const {
                return ptr;
            }

            /** Prefix increment: **/
            ListConstIterator &operator++() {
                ptr = ptr->next;
                return *this;
            }

            /** Postfix increment: **/
            ListConstIterator operator++(int) {
                auto iter = *this;
                ++(*this);
                return iter;
            }

            bool operator==(const ListConstIterator &rhs) const {
                return ptr == rhs.ptr;
            }

            bool operator!=(const ListConstIterator &rhs) const {
                return ptr != rhs.ptr;
            }
        };

    public:
        using Iterator = ListIterator;
        using ConstIterator = ListConstIterator;

        LinkedList() = default;

        // TODO: Copy constructor
        // TODO: Copy assignment operator
        // TODO: Move constructor
        // TODO: Move assignment operator
        // TODO: Destructor

        // TODO: Swap
        // TODO: Reverse
        // TODO: Reverse

        explicit LinkedList(Node<object_type> *p): head {p} {
        }

        LinkedList(std::initializer_list<object_type> args)
        {
            if (not empty(args)) {
                head = new Node<object_type>(*std::begin(args));
                Node<object_type> *node = head;
                for (const object_type *it = std::begin(args) + 1; it != std::end(args); ++it) {
                    node->next = new Node<object_type>(*it);
                    node = node->next;
                }
                tail = node;
            }
        }

        ~LinkedList() {
            /*
            auto node = head;
            while (head) {
                node = head;
                head = head->next;
                delete node;
            }*/

            // std::destroy_n(head, size);

            while (head) {
                delete std::exchange(head, head->next);
            }
        }

        void push_back(object_type val) {
            if (auto last = new Node<object_type>{val}; head) {
                auto prev = tail;
                tail = last;
                prev->next = tail;
            } else {
                tail = head = last;
            }
        }

        /** Iterators support:  **/

        [[nodiscard]]
        inline Iterator begin() noexcept {
            return Iterator {head};

        }

        [[nodiscard]]
        inline Iterator end() noexcept {
            // return Iterator {tail};

            /// In case of single linked list 'tail' <--> The last element
            /// tail->next == NULLPTR
            return Iterator { nullptr };
        }

        [[nodiscard]]
        inline ConstIterator begin() const noexcept {
            return ConstIterator {head};
        }

        [[nodiscard]]
        inline ConstIterator end() const noexcept {
            // return ConstIterator {tail};

            /// In case of single linked list 'tail' <--> The last element
            /// tail->next == NULLPTR
            return ConstIterator { nullptr };
        }
    };

}

namespace LinkedLists::TestsOne
{
    using namespace One;

    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const std::list<T>& list) {
        for (const auto& v: list)
            stream << v << ' ';
        return stream;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const LinkedList<T>& list) {
        for (const auto& v: list)
            stream << v << ' ';
        return stream;
    }

    template<typename T>
    Node<T>* getTestList(T start = 0, T end = 5) {
        Node<T>* root = new Node<T>{start}, *node = root;
        for (T i = start + 1; i < end; ++i) {
            node->next = new Node<T>{i};
            node = node->next;
        }
        return root;
    }



    void PrintContent_Iterator()
    {
        Node<int>* root = getTestList<int>();
        const LinkedList list {root};

        for (auto it: list)
            std::cout << it << " ";
        std::cout << std::endl;

        for (const auto it: list)
            std::cout << it << " ";
        std::cout << std::endl;
    }


    void PushBackElement()
    {
        constexpr int32_t len {10};

        std::list<int> stdList;
        LinkedList<int> list;
        for (int i = 0; i < len; ++i) {
            stdList.push_back(i);
            list.push_back(i);
        }

        std::cout << stdList << std::endl;
        std::cout << list << std::endl;
    }

    void initializer_list()
    {
        {
            std::list<int> stdList{1, 2, 3, 4, 5};
            LinkedList<int> list{1, 2, 3, 4, 5};

            std::cout << stdList << std::endl;
            std::cout << list << std::endl;
        }

        {
            std::list<int> stdList{};
            LinkedList<int> list{};

            std::cout << stdList << std::endl;
            std::cout << list << std::endl;
        }
    }

    void TEST()
    {
        LinkedList<int> list {1,2,3,4,5};
        std::cout << list << std::endl;
    }
}


namespace LinkedLists::Two
{
    template<typename T>
    struct LinkedList
    {
        using object_type = T;
        using reference = object_type&;
        using pointer = object_type*;
        using const_reference = const object_type&;
        using const_pointer = const object_type*;


        struct Node {
            object_type data {};
            Node *next {nullptr};

            Node (object_type v, Node* nxt = nullptr): data {v}, next {nxt} {
            }
        };


        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        Node* head { nullptr };
        Node* tail { nullptr };

        // TODO: Increment
        size_t size {0};

    public:
        class ListIterator {
            using value_type = LinkedList::object_type;
            using reference = LinkedList::reference;
            using pointer = LinkedList::pointer;
            // using self_type = ListIterator;

            Node* ptr;

            template<typename Type>
            friend struct LinkedList;

            [[maybe_unused]]
            explicit ListIterator(Node* p) : ptr{p} {
                //std::cout << "ListIterator!!!\n";
            }

        public:
            reference operator*() const {
                return ptr->data;
            }

            ListIterator &operator++() {
                ptr = ptr->next;
                return *this;
            }

            ListIterator operator++(int) {
                ListIterator iter = *this;
                ++*this;
                return iter;
            }

            bool operator==(const ListIterator &rhs) const {
                return ptr == rhs.ptr;
            }

            bool operator!=(const ListIterator &rhs) const {
                return ptr != rhs.ptr;
            }
        };

        class ListConstIterator {
            using value_type = LinkedList::object_type;
            using const_reference = LinkedList::const_reference;
            using const_pointer = LinkedList::const_pointer;

            Node* ptr;

            template<typename Type>
            friend struct LinkedList;

            [[maybe_unused]]
            explicit ListConstIterator(Node* p) : ptr{p} {
                //std::cout << "ListConstIterator!!!\n";
            }

        public:
            const_reference operator*() const {
                return ptr->data;
            }

            const_pointer operator->() const {
                return ptr;
            }

            /** Prefix increment: **/
            ListConstIterator &operator++() {
                ptr = ptr->next;
                return *this;
            }

            /** Postfix increment: **/
            ListConstIterator operator++(int) {
                auto iter = *this;
                ++(*this);
                return iter;
            }

            bool operator==(const ListConstIterator &rhs) const {
                return ptr == rhs.ptr;
            }

            bool operator!=(const ListConstIterator &rhs) const {
                return ptr != rhs.ptr;
            }
        };

    public:
        using Iterator = ListIterator;
        using ConstIterator = ListConstIterator;

        LinkedList() = default;

        // TODO: Copy constructor
        // TODO: Copy assignment operator
        // TODO: Move constructor
        // TODO: Move assignment operator
        // TODO: Destructor

        // TODO: Swap
        // TODO: Reverse
        // TODO: Reverse

        explicit LinkedList(Node* p): head {p} {
        }

        LinkedList(std::initializer_list<object_type> args)
        {
            if (not empty(args)) {
                head = new Node(*std::begin(args));
                Node *node = head;
                for (const object_type *it = std::begin(args) + 1; it != std::end(args); ++it) {
                    node->next = new Node(*it);
                    node = node->next;
                }
                tail = node;
            }
        }

        ~LinkedList() {
            /*
            auto node = head;
            while (head) {
                node = head;
                head = head->next;
                delete node;
            }*/

            // std::destroy_n(head, size);

            while (head) {
                delete std::exchange(head, head->next);
            }
        }

        void push_back(object_type val) {
            if (auto last = new Node{val}; head) {
                auto prev = tail;
                tail = last;
                prev->next = tail;
            } else {
                tail = head = last;
            }
        }

        /** Iterators support:  **/

        [[nodiscard]]
        inline Iterator begin() noexcept {
            return Iterator {head};

        }

        [[nodiscard]]
        inline Iterator end() noexcept {
            // return Iterator {tail};

            /// In case of single linked list 'tail' <--> The last element
            /// tail->next == NULLPTR
            return Iterator { nullptr };
        }

        [[nodiscard]]
        inline ConstIterator begin() const noexcept {
            return ConstIterator {head};
        }

        [[nodiscard]]
        inline ConstIterator end() const noexcept {
            // return ConstIterator {tail};

            /// In case of single linked list 'tail' <--> The last element
            /// tail->next == NULLPTR
            return ConstIterator { nullptr };
        }
    };

}

namespace LinkedLists::TestsTwo
{
    using namespace Two;

    template<typename T>
    using Node = LinkedList<T>::Node;

    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const std::list<T>& list) {
        for (const auto& v: list)
            stream << v << ' ';
        return stream;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const LinkedList<T>& list) {
        for (const auto& v: list)
            stream << v << ' ';
        return stream;
    }

    template<typename T>
    Node<T>* getTestList(T start = 0, T end = 5) {
        Node<T>* root = new Node<T>{start}, *node = root;
        for (T i = start + 1; i < end; ++i) {
            node->next = new Node<T>{i};
            node = node->next;
        }
        return root;
    }



    void PrintContent_Iterator()
    {
        Node<int>* root = getTestList<int>();
        const LinkedList list {root};

        for (auto it: list)
            std::cout << it << " ";
        std::cout << std::endl;

        for (const auto it: list)
            std::cout << it << " ";
        std::cout << std::endl;
    }


    void PushBackElement()
    {
        constexpr int32_t len {10};

        std::list<int> stdList;
        LinkedList<int> list;
        for (int i = 0; i < len; ++i) {
            stdList.push_back(i);
            list.push_back(i);
        }

        std::cout << stdList << std::endl;
        std::cout << list << std::endl;
    }

    void initializer_list()
    {
        {
            std::list<int> stdList{1, 2, 3, 4, 5};
            LinkedList<int> list{1, 2, 3, 4, 5};

            std::cout << stdList << std::endl;
            std::cout << list << std::endl;
        }

        {
            std::list<int> stdList{};
            LinkedList<int> list{};

            std::cout << stdList << std::endl;
            std::cout << list << std::endl;
        }
    }

    void TEST()
    {
        LinkedList<int> list {1,2,3,4,5};
        std::cout << list << std::endl;
    }
}


void LinkedLists::TEST_ALL()
{
    // TestsOne::PrintContent_Iterator();
    // TestsOne::PushBackElement();
    // TestsOne::initializer_list();
    // TestsOne::TEST();


    // TestsTwo::PrintContent_Iterator();
    // TestsTwo::PushBackElement();
    // TestsTwo::initializer_list();
    TestsTwo::TEST();
};
