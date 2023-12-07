/**============================================================================
Name        : BinaryTree.cpp
Created on  : 29.01.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : BinaryTree data structure implementation
============================================================================**/

#include "BinaryTree.h"

#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include <random>


namespace Utilities
{
    static std::random_device randomDevice{};
    static std::mt19937 generator(randomDevice());

    [[nodiscard]]
    int randomIntegerInRange(int from = 0, int until = 1000) {
        return std::uniform_int_distribution<int>{from, until}(generator);
    }
}


namespace BinaryTree
{
    template<typename T = int>
    struct Node
    {
        using value_type = T;
        using node_pointer = Node<value_type>*;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        value_type data;
        node_pointer left { nullptr };
        node_pointer right { nullptr };

    public:
        Node(value_type d, node_pointer l, node_pointer r) : data {d}, left {l}, right {r} {
        }

        explicit Node(value_type data) : data {data}, left {nullptr}, right {nullptr} {
        }
    };


    // TODO: Concepts -- comparable
    template<typename T = int>
    class BinaryTree
    {
    public:

        using value_type = Node<T>::value_type;
        using node_pointer = Node<T>::node_pointer;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        node_pointer root { nullptr };

    public:

        [[nodiscard]]
        node_pointer getRoot() const {
            return root;
        }

        void insert(value_type value)
        {
            if (nullptr == root) {
                root = new Node<value_type>(value);
                return;
            }

            node_pointer curr {root}, prev { nullptr };
            while (curr) {
                prev = std::exchange(curr, (value < curr->data) ? curr->left : curr->right);
            }

            if (auto newNode = new Node<value_type>(value); value < prev->data)
                prev->left = newNode;
            else
                prev->right = newNode;
        }

        [[nodiscard]]
        bool contains(const value_type& value) const
        {
            node_pointer node = root;
            while (nullptr != node) {
                if (value == node->data)
                    return true;
                else if (value > node->data)
                    node = node->right;
                else
                    node = node->left;
            }
            return false;
        }

        [[nodiscard]]
        node_pointer find(const value_type& value) const
        {
            node_pointer node = root;
            while (nullptr != node) {
                if (value == node->data)
                    return node;
                else if (value > node->data)
                    node = node->right;
                else
                    node = node->left;
            }
            return nullptr;
        }

    public: /** Test funcs **/

        void printInorder()
        {
            std::vector<node_pointer> stack {};
            node_pointer curr = root;

            while (curr || !stack.empty())
            {   /* Reach the left most Node of the curr Node */
                while (curr)
                { /* place pointer to a tree node on the stack before traversing the node's left subtree */
                    stack.push_back(curr);
                    curr = curr->left;
                }

                // Current must be NULL at this point
                curr = stack.back();
                stack.pop_back();

                std::cout << curr->data << " ";

                /* we have visited the node and its left subtree --> Now, it's right subtree's turn */
                curr = curr->right;
            }
        }

        void printBackwards()
        {
            std::vector<node_pointer> stack {};
            node_pointer curr = root;

            while (curr || !stack.empty())
            {
                while (curr) {
                    stack.push_back(curr);
                    curr = curr->right;
                }

                curr = stack.back();
                stack.pop_back();

                std::cout << curr->data << " ";
                curr = curr->left;
            }
        }

        [[nodiscard]]
        bool isBST() const
        {
            std::vector<node_pointer> stack {};
            node_pointer curr = root;

            int previous = std::numeric_limits<int>::min();
            while (nullptr != curr || !stack.empty())
            {   /* Reach the left most Node of the curr Node */
                while (nullptr != curr)
                { /* place pointer to a tree node on the stack before traversing the node's left subtree */
                    stack.push_back(curr);
                    curr = curr->left;
                }

                curr = stack.back(); // Current must be NULL at this point
                stack.pop_back();

                if (previous > curr->data)
                    return false;

                previous = curr->data;
                curr = curr->right;
            }
            return true;
        }
    };
}

namespace BinaryTree::Tests
{
    void CreateAndPrintTree()
    {
        BinaryTree<int> tree;
        for (int i = 0; i < 10; ++i)
            tree.insert(Utilities::randomIntegerInRange(0, 100));

        tree.printInorder();
    }

    void CreateAndPrintTreeBackwards()
    {
        BinaryTree<int> tree;
        for (int i = 0; i < 10; ++i)
            tree.insert(Utilities::randomIntegerInRange(0, 100));

        tree.printBackwards();
    }

    void check_is_BST()
    {
        BinaryTree<int> tree;
        for (int i = 0; i < 100; ++i)
            tree.insert(Utilities::randomIntegerInRange(0, 1000));

        std::cout << std::boolalpha << tree.isBST() << std::endl;
    }

    void checkContains()
    {
        BinaryTree<int> tree;
        tree.insert(7);
        tree.insert(34);
        tree.insert(14);
        tree.insert(25);
        tree.insert(41);

        std::cout << std::boolalpha << tree.contains(25) << std::endl;
        std::cout << std::boolalpha << tree.contains(33) << std::endl;
    }
}

// TODO: find | check contains
// TODO: find MAX | MIN
// TODO: Remove by values
// TODO: FindLowestCommonAncestor
// TODO: Find_N_th_MinElement
// TODO: Rebalance

void BinaryTree::TestAll()
{
    // Tests::CreateAndPrintTree();
    // Tests::CreateAndPrintTreeBackwards();
    // Tests::check_is_BST();
    Tests::checkContains();
}