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

namespace LinkedList {

    template<typename T>
    struct Node {
        T data {};
        Node *next {nullptr};
    };


    template<typename T>
    struct LinkedList {
        Node<T> *head { nullptr };
        Node<T> *tail { nullptr };

    public:
        class ListIterator {
            using value_type = T;
            using reference = T& ;
            using pointer = T* ;
            // using self_type = ListIterator;

            Node<value_type> *ptr;

            template<typename Type>
            friend class LinkedList;

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
                auto iter = *this;
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
            using value_type = T;
            using const_reference = const T& ;
            using const_pointer = const T* ;

            Node<value_type> *ptr;

            template<typename Type>
            friend class LinkedList;

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

        explicit LinkedList(Node<T> *p): head {p} {
        }

        [[nodiscard]]
        inline Iterator begin() {
            return Iterator {head};
        }

        [[nodiscard]]
        inline Iterator end() {
            return Iterator {tail};
        }

        [[nodiscard]]
        inline ConstIterator begin() const {
            return ConstIterator {head};
        }

        [[nodiscard]]
        inline ConstIterator end() const {
            return ConstIterator {tail};
        }
    };


    template<typename T>
    Node<T>* getList() {
        Node<int>* root = new Node<int>{0}, *node = root;
        for (int i = 1; i < 10; ++i) {
            node->next = new Node<int>{i};
            node = node->next;
        }
        return root;
    }
}

void LinkedList::TEST_ALL()
{

    Node<int>* root = getList<int>();
    const LinkedList list {root};

    for (auto it: list)
        std::cout << it << " ";
    std::cout << std::endl;

    for (const auto it: list)
        std::cout << it << " ";
    std::cout << std::endl;

    /*
    Node *node = root;
    while (nullptr != node) {
        std::cout << node->data << std::endl;
        node = node->next;
    }*/

};
