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



namespace BinaryTree
{
    template<typename T = int>
    class Node {
    public:
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


    template<typename T = int>
    class BinaryTree {
    public:
        using value_type = Node<T>::value_type;
        using node_pointer = Node<T>::node_pointer;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

    // private:
    public:
        node_pointer root { nullptr };

        /*
        node_pointer insert(value_type value,
                            node_pointer node) {
            if (nullptr == node) {
                node = new Node<value_type>(value);
            }
            else if (value < node->data) {
                node->left = insert(value, node->left);
            }
            else if (value > node->data) {
                node->right = insert(value, node->right);
            }
            return node;
        }
        */


    public:

        node_pointer getRoot() const {
            return root;
        }

        /*
        void insert(value_type value) {
            root = insert(value, root);
        }*/


        /*
        node_pointer insert(value_type value,
                            node_pointer node) {
            if (nullptr == node) {
                node = new Node<value_type>(value);
                return node;
            }

            node_pointer curr { node }, prev { nullptr };
            while (curr) {
                prev = curr;
                if (value < curr->data)
                    curr = curr->left;
                else
                    curr = curr->right;
            }

            // TODO: Fixme 2 new
            if (value < prev->data)
                prev->left = new Node<value_type>(value);
            else
                prev->right = new Node<value_type>(value);
            return prev;

            //   BinaryTree<int> tree;
            //   tree.root = tree.insert(10, tree.root);
            //   tree.insert(4, tree.root);
            //   tree.insert(20, tree.root);
            //   tree.insert(8, tree.root);
        }
        */

        void insert(value_type value) {
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

    public: /** Test funcs **/

        void inorder(const node_pointer node) {
            if (nullptr == node)
                return;
            inorder(node->left);
            std::cout << node->data << " ";
            inorder(node->right);
        }

        void display() {
            inorder(root);
            std::cout << std::endl;
        }
    };
}

namespace BinaryTree::Tests
{
    void CreateAndPrintTree()
    {
        /*
        BinaryTree<int> tree;

        tree.insert(10);
        tree.insert(4);
        tree.insert(20);
        tree.insert(8);
        tree.insert(30);

        tree.display();
        */
    }

    void CreateAndPrintTreeTEst()
    {
        BinaryTree<int> tree;

        tree.insert(10);
        tree.insert(4);
        tree.insert(20);
        tree.insert(8);

        tree.display();
    }
}

void BinaryTree::TestAll()
{
    //  Tests::CreateAndPrintTree();
    Tests::CreateAndPrintTreeTEst();
}